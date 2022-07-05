CREATE TABLE IF NOT EXISTS `premium_rewarded_characters` (
    `account_id` INT(10) UNSIGNED NOT NULL,
    `realm_id` INT(10) UNSIGNED NOT NULL,
    `character_guid` INT(10) UNSIGNED NOT NULL,
    `items` INT(10) UNSIGNED NOT NULL,
    PRIMARY KEY (`account_id`, `realm_id`, `character_guid`) USING BTREE
) COLLATE='utf8mb4_general_ci'
ENGINE=InnoDB
;
