-- Links an AI chat context to a planner step.
CREATE TABLE IF NOT EXISTS chat_step_links (
    id         UUID PRIMARY KEY
                   DEFAULT gen_random_uuid(),
    user_id    UUID NOT NULL
                   REFERENCES users(id)
                   ON DELETE CASCADE,
    step_id    TEXT NOT NULL,
    context    TEXT NOT NULL DEFAULT '',
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_chat_step_links_user_step
    ON chat_step_links(user_id, step_id);
