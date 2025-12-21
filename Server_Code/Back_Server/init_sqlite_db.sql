-- SQLite Database Schema for MindCarrer
-- Converted from MySQL schema

-- Users table
CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    phone TEXT NOT NULL UNIQUE,
    pwd_hash TEXT NOT NULL,
    name TEXT NOT NULL,
    avatar_index INTEGER DEFAULT 0,
    status INTEGER DEFAULT 1,
    created_at TEXT DEFAULT (datetime('now', 'localtime')),
    updated_at TEXT DEFAULT (datetime('now', 'localtime'))
);

CREATE INDEX IF NOT EXISTS idx_users_phone ON users(phone);

-- SMS verification codes table
CREATE TABLE IF NOT EXISTS auth_sms_codes (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    phone TEXT NOT NULL,
    code TEXT NOT NULL,
    purpose TEXT NOT NULL,
    expire_at TEXT NOT NULL,
    verified INTEGER DEFAULT 0,
    created_at TEXT DEFAULT (datetime('now', 'localtime'))
);

CREATE INDEX IF NOT EXISTS idx_sms_phone ON auth_sms_codes(phone);
CREATE INDEX IF NOT EXISTS idx_sms_code ON auth_sms_codes(code);
CREATE INDEX IF NOT EXISTS idx_sms_created ON auth_sms_codes(created_at);

-- Questionnaires table
CREATE TABLE IF NOT EXISTS questionnaires (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    code TEXT NOT NULL UNIQUE,
    name TEXT NOT NULL,
    type TEXT NOT NULL,
    status INTEGER DEFAULT 1,
    description TEXT,
    created_at TEXT DEFAULT (datetime('now', 'localtime')),
    updated_at TEXT DEFAULT (datetime('now', 'localtime'))
);

CREATE INDEX IF NOT EXISTS idx_questionnaires_code ON questionnaires(code);
CREATE INDEX IF NOT EXISTS idx_questionnaires_type ON questionnaires(type);

-- Questions table
CREATE TABLE IF NOT EXISTS questions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    questionnaire_id INTEGER NOT NULL,
    stem TEXT NOT NULL,
    option_json TEXT,
    enable_flag INTEGER DEFAULT 1,
    created_at TEXT DEFAULT (datetime('now', 'localtime')),
    updated_at TEXT DEFAULT (datetime('now', 'localtime')),
    FOREIGN KEY (questionnaire_id) REFERENCES questionnaires(id)
);

CREATE INDEX IF NOT EXISTS idx_questions_questionnaire ON questions(questionnaire_id);

-- Assessments table
CREATE TABLE IF NOT EXISTS assessments (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    questionnaire_id INTEGER NOT NULL,
    start_at TEXT NOT NULL,
    submit_at TEXT,
    total_score REAL DEFAULT 0,
    risk_level TEXT DEFAULT 'green',
    source TEXT DEFAULT 'daily',
    client_tag TEXT,
    created_at TEXT DEFAULT (datetime('now', 'localtime')),
    FOREIGN KEY (user_id) REFERENCES users(id),
    FOREIGN KEY (questionnaire_id) REFERENCES questionnaires(id)
);

CREATE INDEX IF NOT EXISTS idx_assessments_user ON assessments(user_id);
CREATE INDEX IF NOT EXISTS idx_assessments_tag ON assessments(client_tag);

-- Answers table
CREATE TABLE IF NOT EXISTS answers (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    assessment_id INTEGER NOT NULL,
    question_id INTEGER NOT NULL,
    option_code TEXT,
    option_score REAL DEFAULT 0,
    answer_text TEXT,
    created_at TEXT DEFAULT (datetime('now', 'localtime')),
    FOREIGN KEY (assessment_id) REFERENCES assessments(id),
    FOREIGN KEY (question_id) REFERENCES questions(id)
);

CREATE INDEX IF NOT EXISTS idx_answers_assessment ON answers(assessment_id);

-- Blogs table
CREATE TABLE IF NOT EXISTS blogs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    content TEXT NOT NULL,
    is_anonymous INTEGER DEFAULT 0,
    created_at TEXT DEFAULT (datetime('now', 'localtime')),
    updated_at TEXT DEFAULT (datetime('now', 'localtime')),
    FOREIGN KEY (user_id) REFERENCES users(id)
);

CREATE INDEX IF NOT EXISTS idx_blogs_user ON blogs(user_id);
CREATE INDEX IF NOT EXISTS idx_blogs_created ON blogs(created_at);

-- Messages table
CREATE TABLE IF NOT EXISTS messages (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    sender_id INTEGER NOT NULL,
    receiver_id INTEGER NOT NULL,
    content TEXT NOT NULL,
    is_read INTEGER DEFAULT 0,
    created_at TEXT DEFAULT (datetime('now', 'localtime')),
    FOREIGN KEY (sender_id) REFERENCES users(id),
    FOREIGN KEY (receiver_id) REFERENCES users(id)
);

CREATE INDEX IF NOT EXISTS idx_messages_sender ON messages(sender_id);
CREATE INDEX IF NOT EXISTS idx_messages_receiver ON messages(receiver_id);
CREATE INDEX IF NOT EXISTS idx_messages_created ON messages(created_at);

-- Indicators table (for mental health tracking)
CREATE TABLE IF NOT EXISTS indicators (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    dimension_code TEXT NOT NULL,
    last_score REAL DEFAULT 0,
    avg_7d REAL DEFAULT 0,
    avg_30d REAL DEFAULT 0,
    trend TEXT DEFAULT 'F',
    updated_at TEXT DEFAULT (datetime('now', 'localtime')),
    FOREIGN KEY (user_id) REFERENCES users(id)
);

CREATE INDEX IF NOT EXISTS idx_indicators_user ON indicators(user_id);
CREATE INDEX IF NOT EXISTS idx_indicators_dimension ON indicators(dimension_code);

-- Risk events table
CREATE TABLE IF NOT EXISTS risk_events (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    assessment_id INTEGER,
    level TEXT NOT NULL,
    trigger_desc TEXT,
    action_taken TEXT,
    created_at TEXT DEFAULT (datetime('now', 'localtime')),
    FOREIGN KEY (user_id) REFERENCES users(id),
    FOREIGN KEY (assessment_id) REFERENCES assessments(id)
);

CREATE INDEX IF NOT EXISTS idx_risk_events_user ON risk_events(user_id);
CREATE INDEX IF NOT EXISTS idx_risk_events_created ON risk_events(created_at);

-- Insert sample questionnaire data
INSERT OR IGNORE INTO questionnaires (id, code, name, type, status, description) 
VALUES (1, 'DAILY_PHQGAD', 'Daily Mental Health Check', 'daily', 1, 'Daily quick assessment for mood and anxiety');

-- Insert sample questions
INSERT OR IGNORE INTO questions (id, questionnaire_id, stem, option_json, enable_flag) VALUES
(1, 1, 'In the past 24h, how was your overall mood?', '[{"code":"A","label":"Not at all","score":1},{"code":"B","label":"Several days","score":2},{"code":"C","label":"More than half the days","score":3},{"code":"D","label":"Nearly every day","score":4}]', 1),
(2, 1, 'In the past 24h, did you enjoy things you usually like?', '[{"code":"A","label":"Not at all","score":1},{"code":"B","label":"Several days","score":2},{"code":"C","label":"More than half the days","score":3},{"code":"D","label":"Nearly every day","score":4}]', 1),
(3, 1, 'In the past 24h, did you feel nervous or hard to relax?', '[{"code":"A","label":"Not at all","score":1},{"code":"B","label":"Several days","score":2},{"code":"C","label":"More than half the days","score":3},{"code":"D","label":"Nearly every day","score":4}]', 1),
(4, 1, 'In the past 24h, did you have trouble falling or staying asleep?', '[{"code":"A","label":"Not at all","score":1},{"code":"B","label":"Several days","score":2},{"code":"C","label":"More than half the days","score":3},{"code":"D","label":"Nearly every day","score":4}]', 1),
(5, 1, 'In the past 24h, did you feel tired or low on energy?', '[{"code":"A","label":"Not at all","score":1},{"code":"B","label":"Several days","score":2},{"code":"C","label":"More than half the days","score":3},{"code":"D","label":"Nearly every day","score":4}]', 1),
(6, 1, 'In the past 24h, were you worrying too much?', '[{"code":"A","label":"Not at all","score":1},{"code":"B","label":"Several days","score":2},{"code":"C","label":"More than half the days","score":3},{"code":"D","label":"Nearly every day","score":4}]', 1),
(7, 1, 'In the past 24h, how often did you blame yourself or feel hopeless?', '[{"code":"A","label":"Not at all","score":1},{"code":"B","label":"Several days","score":2},{"code":"C","label":"More than half the days","score":3},{"code":"D","label":"Nearly every day","score":4}]', 1);

-- Commit changes
COMMIT;