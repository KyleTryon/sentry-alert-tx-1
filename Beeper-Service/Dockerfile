# syntax=docker/dockerfile:1.7-labs
# Hyper-optimized multi-stage build for minimal image size and maximum caching

# Build stage - Use Node.js 22 LTS (Jod) - Active LTS as of August 2025
FROM node:22-alpine AS builder

# Set working directory
WORKDIR /app

# Copy package files ONLY (best caching - these change least frequently)
COPY package.json package-lock.json* ./

# Install dependencies
# 1) Install ONLY production deps into prod_node_modules
# 2) Install full deps (incl. dev) for building
RUN if [ -f package-lock.json ]; then \
      npm ci --omit=dev --no-audit --no-fund; \
    else \
      npm install --omit=dev --no-audit --no-fund; \
    fi && \
    cp -R node_modules prod_node_modules && \
    if [ -f package-lock.json ]; then \
      npm ci --no-audit --no-fund; \
    else \
      npm install --no-audit --no-fund; \
    fi

# Copy source files (changes more frequently than deps)
COPY tsconfig.json ./
COPY src/ ./src/

# Build application
RUN npm run build && \
    # Remove source maps and other dev artifacts to reduce size
    find dist -name "*.map" -delete

# Production stage - Ultra-minimal distroless image with Node.js 22 LTS (Jod)
FROM gcr.io/distroless/nodejs22-debian12:nonroot AS production

# Set working directory
WORKDIR /app

# Copy only production dependencies and built app from builder
COPY --from=builder /app/prod_node_modules ./node_modules
COPY --from=builder /app/dist ./dist
COPY --from=builder /app/package.json ./

# Use distroless nonroot user (uid 65532)
USER 65532:65532

# Expose port
EXPOSE 3000

# No shell available in distroless, so simple command
CMD ["dist/index.js"]
