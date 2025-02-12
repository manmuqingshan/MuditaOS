-- Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

-- Message: Add bedside lamp parameters
-- Revision: 90970372-6151-4de3-91dd-f1c8684bc6f4
-- Create Date: 2024-08-08 13:56:09

UPDATE OR IGNORE settings_tab
SET path = 'br_state'
WHERE path = '\EventManager\\br_state';

UPDATE OR IGNORE settings_tab
SET path = 'br_auto_mode'
WHERE path = '\EventManager\\br_auto_mode';

UPDATE OR IGNORE settings_tab
SET path = 'br_level'
WHERE path = '\EventManager\\br_level';

INSERT OR IGNORE INTO settings_tab (path, value) VALUES
       ('br_bedside_level', '58.0f'),
       ('br_bedside_time', '10');

-- Message: Enable relaxation fade effect by default
-- Revision: 39114449-29a6-42a3-a239-57b8c726501c
-- Create Date: 2024-09-05 11:37:44

-- Insert SQL here

INSERT OR IGNORE INTO settings_tab (path, value) VALUES
       ('relaxation_fade_active', '1');

