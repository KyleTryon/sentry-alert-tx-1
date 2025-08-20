import { createHmac } from 'crypto';

/**
 * Verifies the Sentry webhook signature to ensure authenticity
 * Based on Sentry's documentation: https://docs.sentry.io/organization/integrations/integration-platform/webhooks/
 */
export function verifySignature(
  payload: string | Buffer,
  signature: string,
  secret: string
): boolean {
  if (!secret) {
    // If no secret is configured, skip verification
    return true;
  }

  try {
    const hmac = createHmac('sha256', secret);
    if (typeof payload === 'string') {
      hmac.update(payload, 'utf8');
    } else {
      hmac.update(payload);
    }
    const computedSignature = hmac.digest('hex');
    
    // Constant-time comparison to prevent timing attacks
    return constantTimeCompare(signature, computedSignature);
  } catch (error) {
    console.error('Error verifying signature:', error);
    return false;
  }
}

/**
 * Constant-time string comparison to prevent timing attacks
 */
function constantTimeCompare(a: string, b: string): boolean {
  if (a.length !== b.length) {
    return false;
  }

  let result = 0;
  for (let i = 0; i < a.length; i++) {
    result |= a.charCodeAt(i) ^ b.charCodeAt(i);
  }

  return result === 0;
}

/**
 * Validates that the webhook timestamp is within an acceptable time window
 * to prevent replay attacks
 */
export function validateTimestamp(
  timestamp: string,
  toleranceSeconds: number = 300 // 5 minutes
): boolean {
  try {
    const webhookTime = parseInt(timestamp, 10) * 1000; // Convert to milliseconds
    const now = Date.now();
    const diff = Math.abs(now - webhookTime);
    
    return diff <= toleranceSeconds * 1000;
  } catch (error) {
    console.error('Error validating timestamp:', error);
    return false;
  }
}
