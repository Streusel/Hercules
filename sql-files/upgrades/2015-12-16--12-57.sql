#1450241859

-- This file is part of Hercules.
-- http://herc.ws - http://github.com/HerculesWS/Hercules
--
-- Copyright (C) 2015-2025 Hercules Dev Team
--
-- Hercules is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <http://www.gnu.org/licenses/>.

ALTER TABLE `char` MODIFY COLUMN `max_hp` INT NOT NULL DEFAULT '0';
ALTER TABLE `char` MODIFY COLUMN `max_sp` INT NOT NULL DEFAULT '0';
ALTER TABLE `char` MODIFY COLUMN `hp` INT NOT NULL DEFAULT '0';
ALTER TABLE `char` MODIFY COLUMN `sp` INT NOT NULL DEFAULT '0';

INSERT INTO `sql_updates` (`timestamp`) VALUES (1450241859);
