DELETE FROM `command` WHERE `name` IN ('premium account add', 'premium account remove');
INSERT INTO `command` (`name`, `security`, `help`) VALUES 
('premium account add', 2, 'Syntax: .premium account add [$account]\r\n\r\nEnable premium features for the account'),
('premium account remove', 2, 'Syntax: .premium account remove [$account]\r\n\r\nDisable premium features for the account');
