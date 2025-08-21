import { z } from 'zod';

// Sentry webhook headers schema
export const SentryWebhookHeadersSchema = z.object({
  'content-type': z.literal('application/json'),
  'request-id': z.string().uuid().optional(), // Optional - Sentry doesn't always send this
  'sentry-hook-resource': z.string(), // Changed to string to accept any resource type like 'error.created', 'issue.resolved', etc.
  'sentry-hook-timestamp': z.string(),
  'sentry-hook-signature': z.string().optional(), // Optional - only sent when webhook verification is enabled
});

// Actor schema
export const ActorSchema = z.object({
  type: z.enum(['user', 'application']),
  id: z.union([z.string(), z.number()]),
  name: z.string(),
});

// Installation schema
export const InstallationSchema = z.object({
  uuid: z.string().uuid(),
});

// Event schema for issue alerts
export const EventSchema = z.object({
  id: z.string(),
  title: z.string(),
  message: z.string().optional(),
  level: z.enum(['debug', 'info', 'warning', 'error', 'fatal']),
  timestamp: z.number(),
  url: z.string().url().optional(),
  web_url: z.string().url().optional(),
  issue_url: z.string().url().optional(),
  issue_id: z.string().optional(),
  platform: z.string().optional(),
  environment: z.string().optional(),
  release: z.string().optional(),
  tags: z.array(z.tuple([z.string(), z.string()])).optional(),
  user: z.object({
    id: z.string().optional(),
    username: z.string().optional(),
    email: z.string().email().optional(),
    ip_address: z.string().optional(),
  }).optional(),
  contexts: z.record(z.any()).optional(),
  fingerprint: z.array(z.string()).optional(),
  metadata: z.record(z.any()).optional(),
});

// Issue Alert specific data
export const IssueAlertSchema = z.object({
  title: z.string(),
  settings: z.array(z.object({
    name: z.string(),
    value: z.any(),
  })).optional(),
});

// Base webhook payload schema
export const BaseWebhookPayloadSchema = z.object({
  action: z.string(),
  installation: InstallationSchema,
  actor: ActorSchema.optional(),
  data: z.record(z.any()),
});

// Issue Alert webhook payload
export const IssueAlertWebhookPayloadSchema = BaseWebhookPayloadSchema.extend({
  action: z.literal('triggered'),
  data: z.object({
    event: EventSchema,
    triggered_rule: z.string(),
    issue_alert: IssueAlertSchema.optional(),
  }),
});

// Installation webhook payload
export const InstallationWebhookPayloadSchema = BaseWebhookPayloadSchema.extend({
  action: z.enum(['created', 'deleted']),
  data: z.object({
    installation: z.object({
      status: z.string(),
      organization: z.object({
        slug: z.string(),
      }),
      app: z.object({
        uuid: z.string().uuid(),
        slug: z.string(),
      }),
      code: z.string(),
      uuid: z.string().uuid(),
    }),
  }),
});

// Export types
export type SentryWebhookHeaders = z.infer<typeof SentryWebhookHeadersSchema>;
export type Actor = z.infer<typeof ActorSchema>;
export type Installation = z.infer<typeof InstallationSchema>;
export type Event = z.infer<typeof EventSchema>;
export type IssueAlert = z.infer<typeof IssueAlertSchema>;
export type BaseWebhookPayload = z.infer<typeof BaseWebhookPayloadSchema>;
export type IssueAlertWebhookPayload = z.infer<typeof IssueAlertWebhookPayloadSchema>;
export type InstallationWebhookPayload = z.infer<typeof InstallationWebhookPayloadSchema>;

// Union type for all webhook payloads
export type SentryWebhookPayload = IssueAlertWebhookPayload | InstallationWebhookPayload | BaseWebhookPayload;
