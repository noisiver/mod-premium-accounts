CREATE TABLE IF NOT EXISTS `premium_accounts` (
    `account_id` INT(10) UNSIGNED NOT NULL,
    `enabled_items` INT(10) UNSIGNED NOT NULL DEFAULT 0,
    PRIMARY KEY (`account_id`) USING BTREE
) COLLATE='utf8mb4_general_ci'
ENGINE=InnoDB
;
