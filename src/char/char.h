/**
 * This file is part of Hercules.
 * http://herc.ws - http://github.com/HerculesWS/Hercules
 *
 * Copyright (C) 2012-2025 Hercules Dev Team
 * Copyright (C) Athena Dev Teams
 *
 * Hercules is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef CHAR_CHAR_H
#define CHAR_CHAR_H

#include "common/hercules.h"
#include "common/core.h" // CORE_ST_LAST
#include "common/db.h"
#include "common/mmo.h"
#include "common/chunked/rfifo.h"

/* Forward Declarations */
struct config_setting_t; // common/conf.h
struct config_t; // common/conf.h

enum E_CHARSERVER_ST {
	CHARSERVER_ST_RUNNING = CORE_ST_LAST,
	CHARSERVER_ST_SHUTDOWN,
	CHARSERVER_ST_LAST
};

/* The unusual enum values were chosen to (temporarily) match the magic numbers
 * encoded in an int in the previous implementation, to ease migration. In the
 * future they may be remapped to less unusual values. */
enum online_char_state {
	OCS_UNKNOWN = -2,
	OCS_NOT_CONNECTED = -1,
	OCS_CONNECTED = 0,
};

struct char_session_data {
	bool auth; // whether the session is authed or not
	int account_id, login_id1, login_id2, sex;
	int found_char[MAX_CHARS]; // ids of chars on this account
	time_t unban_time[MAX_CHARS]; // char unban time array
	char email[40]; // e-mail (default: a@a.com) by [Yor]
	time_t expiration_time; // # of seconds 1/1/1970 (timestamp): Validity limit of the account (0 = unlimited)
	int group_id; // permission
	uint8 char_slots;
	uint32 version;
	uint8 clienttype;
	char pincode[4+1];
	uint32 pincode_seed;
	uint16 pincode_try;
	uint32 pincode_change;
	char new_name[NAME_LENGTH];
	char birthdate[10+1];  // YYYY-MM-DD
};

struct online_char_data {
	int account_id;
	int char_id;
	int fd;
	int waiting_disconnect;
	enum online_char_state mapserver_connection;
	int pincode_enable;
	struct online_char_data2 *data;
};

struct online_char_data2 {
	int emblem_guild_id;
	bool emblem_gif;
	struct fifo_chunk_buf emblem_data;
};

struct mmo_map_server {
	int fd;
	uint32 ip;
	uint16 port;
	int users;
	VECTOR_DECL(uint16) maps;
};

#define DEFAULT_CHAR_AUTOSAVE_INTERVAL (300*1000)

enum inventory_table_type {
	TABLE_INVENTORY,
	TABLE_CART,
	TABLE_STORAGE,
	TABLE_GUILD_STORAGE,
};

struct char_auth_node {
	int account_id;
	int char_id;
	uint32 login_id1;
	uint32 login_id2;
	uint32 ip;
	int sex;
	time_t expiration_time; // # of seconds 1/1/1970 (timestamp): Validity limit of the account (0 = unlimited)
	int group_id;
	unsigned changing_mapservers : 1;
};

/**
 * char interface
 **/
struct char_interface {
	struct mmo_map_server map_server;
	int login_fd;
	int char_fd;
	struct DBMap *online_char_db; // int account_id -> struct online_char_data*
	struct DBMap *char_db_;
	char userid[NAME_LENGTH];
	char passwd[NAME_LENGTH];
	char server_name[20];
	uint32 ip;
	uint16 port;
	int server_type;
	int16 new_display; ///< Display 'New' in the server list.

	char *CHAR_CONF_NAME;
	char *NET_CONF_NAME; ///< Network config filename
	char *SQL_CONF_NAME;
	char *INTER_CONF_NAME;

	bool show_save_log; ///< Show loading/saving messages.
	bool enable_logs;   ///< Whether to log char server operations.

	char db_path[256]; //< Database directory (db)

	int (*waiting_disconnect) (int tid, int64 tick, int id, intptr_t data);
	int (*delete_char_sql) (int char_id);
	struct DBData (*create_online_char_data) (union DBKey key, va_list args);
	void (*set_account_online) (int account_id, bool standalone);
	void (*set_account_offline) (int account_id);
	void (*set_char_charselect) (int account_id);
	void (*set_char_online) (bool is_initializing, int char_id, int account_id, bool standalone);
	void (*set_char_offline) (int char_id, int account_id);
	int (*db_setoffline) (union DBKey key, struct DBData *data, va_list ap);
	int (*db_kickoffline) (union DBKey key, struct DBData *data, va_list ap);
	void (*set_login_all_offline) (void);
	void (*set_all_offline) (bool for_shutdown);
	void (*set_all_offline_sql) (void);
	struct DBData (*create_charstatus) (union DBKey key, va_list args);
	int (*mmo_char_tosql) (int char_id, struct mmo_charstatus* p);
	int (*getitemdata_from_sql) (struct item *items, int max, int guid, enum inventory_table_type table);
	int (*memitemdata_to_sql) (const struct item items[], int current_size, int guid, enum inventory_table_type table);
	int (*mmo_gender) (const struct char_session_data *sd, const struct mmo_charstatus *p, char sex);
	int (*mmo_chars_fromsql) (struct char_session_data* sd, uint8* buf, int *count);
	int (*mmo_char_fromsql) (int char_id, struct mmo_charstatus* p, bool load_everything);
	int (*mmo_char_sql_init) (void);
	bool (*char_slotchange) (struct char_session_data *sd, int fd, unsigned short from, unsigned short to);
	int (*rename_char_sql) (struct char_session_data *sd, int char_id);
	bool (*name_exists) (const char *name, const char *esc_name);
	int (*check_char_name) (const char *name, const char *esc_name);
	int (*make_new_char_sql) (struct char_session_data *sd, const char *name_, int str, int agi, int vit, int int_, int dex, int luk, int slot, int hair_color, int hair_style, int starting_job, uint8 sex);
	int (*divorce_char_sql) (int partner_id1, int partner_id2);
	int (*count_users) (void);
	int (*mmo_char_tobuf) (uint8* buffer, struct mmo_charstatus* p);
	void (*send_HC_ACK_CHARINFO_PER_PAGE) (int fd, struct char_session_data *sd);
	void (*send_HC_ACK_CHARINFO_PER_PAGE_tail) (int fd, struct char_session_data *sd);
	void (*mmo_char_send_ban_list) (int fd, struct char_session_data *sd);
	void (*mmo_char_send_slots_info) (int fd, struct char_session_data* sd);
	int (*mmo_char_send_characters) (int fd, struct char_session_data* sd);
	int (*char_married) (int pl1, int pl2);
	int (*char_child) (int parent_id, int child_id);
	int (*char_family) (int cid1, int cid2, int cid3);
	void (*disconnect_player) (int account_id);
	void (*authfail_fd) (int fd, int type);
	void (*request_account_data) (int account_id);
	void (*auth_ok) (int fd, struct char_session_data *sd);
	void (*ping_login_server) (int fd);
	int (*parse_fromlogin_connection_state) (int fd);
	void (*auth_error) (int fd, unsigned char flag);
	void (*parse_fromlogin_auth_state) (int fd);
	void (*parse_fromlogin_account_data) (int fd);
	void (*parse_fromlogin_login_pong) (int fd);
	void (*changesex) (int account_id, int sex);
	int (*parse_fromlogin_changesex_reply) (int fd);
	void (*parse_fromlogin_account_reg2) (int fd);
	void (*parse_fromlogin_ban) (int fd);
	void (*parse_fromlogin_kick) (int fd);
	void (*update_ip) (int fd);
	void (*parse_fromlogin_update_ip) (int fd);
	void (*parse_fromlogin_accinfo2_failed) (int fd);
	void (*parse_fromlogin_accinfo2_ok) (int fd);
	int (*parse_fromlogin) (int fd);
	int (*request_accreg2) (int account_id, int char_id);
	void (*global_accreg_to_login_start) (int account_id, int char_id);
	void (*global_accreg_to_login_send) (void);
	void (*global_accreg_to_login_add) (const char *key, unsigned int index, intptr_t val, bool is_string);
	void (*read_fame_list) (void);
	int (*send_fame_list) (void);
	void (*update_fame_list) (int type, int index, int fame);
	int (*loadName) (int char_id, char* name);
	void (*parse_frommap_datasync) (int fd);
	void (*parse_frommap_skillid2idx) (int fd);
	void (*map_received_ok) (int fd);
	void (*parse_frommap_map_names) (int fd);
	void (*send_scdata) (int fd, int aid, int cid);
	void (*parse_frommap_request_scdata) (int fd);
	void (*parse_frommap_set_users_count) (int fd);
	void (*parse_frommap_set_users) (int fd);
	void (*save_character_ack) (int fd, int aid, int cid);
	void (*parse_frommap_save_character) (int fd);
	void (*select_ack) (int fd, int account_id, uint8 flag);
	void (*parse_frommap_char_select_req) (int fd);
	void (*parse_frommap_remove_friend) (int fd);
	void (*char_name_ack) (int fd, int char_id);
	void (*parse_frommap_char_name_request) (int fd);
	void (*parse_frommap_change_email) (int fd);
	void (*ban) (int account_id, int char_id, time_t *unban_time, short year, short month, short day, short hour, short minute, short second);
	void (*unban) (int char_id, int *result);
	void (*ask_name_ack) (int fd, int acc, const char* name, int type, int result);
	int (*changecharsex) (int char_id, int sex);
	void (*parse_frommap_change_account) (int fd);
	void (*parse_frommap_fame_list) (int fd);
	void (*parse_frommap_divorce_char) (int fd);
	void (*parse_frommap_ragsrvinfo) (int fd);
	void (*parse_frommap_set_char_offline) (int fd);
	void (*parse_frommap_set_all_offline) (int fd);
	void (*parse_frommap_set_char_online) (int fd);
	void (*parse_frommap_build_fame_list) (int fd);
	void (*parse_frommap_save_status_change_data) (int fd);
	void (*send_pong) (int fd);
	void (*parse_frommap_ping) (int fd);
	void (*map_auth_ok) (int fd, int account_id, struct char_auth_node* node, struct mmo_charstatus* cd);
	void (*map_auth_failed) (int fd, int account_id, int char_id, int login_id1, char sex, uint32 ip);
	void (*parse_frommap_auth_request) (int fd);
	void (*parse_frommap_update_ip) (int fd);
	void (*parse_frommap_scdata_update) (int fd);
	void (*parse_frommap_scdata_delete) (int fd);
	int (*parse_frommap) (int fd);
	bool (*mapserver_has_map) (unsigned short map);
	int (*mapif_init) (int fd);
	uint32 (*lan_subnet_check) (uint32 ip);
	void (*delete2_ack) (int fd, int char_id, uint32 result, time_t delete_date);
	void (*delete2_accept_actual_ack) (int fd, int char_id, uint32 result);
	void (*delete2_accept_ack) (int fd, int char_id, uint32 result);
	void (*delete2_cancel_ack) (int fd, int char_id, uint32 result);
	void (*delete2_req) (int fd, struct char_session_data* sd);
	void (*delete2_accept) (int fd, struct char_session_data* sd);
	void (*delete2_cancel) (int fd, struct char_session_data* sd);
	void (*send_account_id) (int fd, int account_id);
	void (*parse_char_connect) (int fd, struct char_session_data* sd, uint32 ipl);
	void (*send_map_info) (int fd, uint32 subnet_map_ip, struct mmo_charstatus *cd, char *dnsHost);
	void (*send_wait_char_server) (int fd);
	bool (*find_available_map_fallback) (struct mmo_charstatus *cd);
	void (*parse_char_select) (int fd, struct char_session_data* sd, uint32 ipl);
	void (*creation_failed) (int fd, int result);
	void (*creation_ok) (int fd, struct mmo_charstatus *char_dat);
	void (*parse_char_create_new_char) (int fd, struct char_session_data* sd);
	void (*delete_char_failed) (int fd, int flag);
	void (*delete_char_ok) (int fd);
	void (*parse_char_delete_char) (int fd, struct char_session_data* sd, unsigned short cmd);
	void (*parse_char_ping) (int fd);
	void (*allow_rename) (int fd, int flag);
	void (*parse_char_rename_char) (int fd, struct char_session_data* sd);
	void (*parse_char_rename_char2) (int fd, struct char_session_data* sd);
	void (*rename_char_ack) (int fd, int flag);
	void (*parse_char_rename_char_confirm) (int fd, struct char_session_data* sd);
	void (*captcha_notsupported) (int fd);
	void (*parse_char_request_captcha) (int fd);
	void (*parse_char_check_captcha) (int fd);
	void (*parse_char_delete2_req) (int fd, struct char_session_data* sd);
	void (*parse_char_delete2_accept) (int fd, struct char_session_data* sd);
	void (*parse_char_delete2_cancel) (int fd, struct char_session_data* sd);
	void (*login_map_server_ack) (int fd, uint8 flag);
	void (*parse_char_login_map_server) (int fd, uint32 ipl);
	void (*parse_char_pincode_check) (int fd, struct char_session_data* sd);
	void (*parse_char_pincode_window) (int fd, struct char_session_data* sd);
	void (*parse_char_pincode_change) (int fd, struct char_session_data* sd);
	void (*parse_char_pincode_first_pin) (int fd, struct char_session_data* sd);
	void (*parse_char_request_chars) (int fd, struct char_session_data* sd);
	void (*change_character_slot_ack) (int fd, bool ret);
	void (*parse_char_move_character) (int fd, struct char_session_data* sd);
	int (*parse_char_unknown_packet) (int fd, uint32 ipl);
	int (*parse_char) (int fd);
	int (*broadcast_user_count) (int tid, int64 tick, int id, intptr_t data);
	int (*send_accounts_tologin_sub) (union DBKey key, struct DBData *data, va_list ap);
	int (*send_accounts_tologin) (int tid, int64 tick, int id, intptr_t data);
	int (*check_connect_login_server) (int tid, int64 tick, int id, intptr_t data);
	int (*online_data_cleanup_sub) (union DBKey key, struct DBData *data, va_list ap);
	int (*online_data_cleanup) (int tid, int64 tick, int id, intptr_t data);
	void (*change_sex_sub) (int sex, int acc, int char_id, int class, int guild_id);
	void (*online_char_destroy) (struct online_char_data *character);
	int (*online_char_destroy_sub) (union DBKey key, struct DBData *data, va_list ap);
	void (*ensure_online_char_data) (struct online_char_data *character);
	void (*clean_online_char_emblem_data) (struct online_char_data *character);

	bool (*sql_config_read) (const char *filename, bool imported);
	bool (*sql_config_read_registry) (const char *filename, const struct config_t *config, bool imported);
	bool (*sql_config_read_pc) (const char *filename, const struct config_t *config, bool imported);
	bool (*sql_config_read_guild) (const char *filename, const struct config_t *config, bool imported);
	bool (*config_read) (const char *filename, bool imported);
	bool (*config_read_database) (const char *filename, const struct config_t *config, bool imported);
	bool (*config_read_console) (const char *filename, const struct config_t *config, bool imported);
	bool (*config_read_player_fame) (const char *filename, const struct config_t *config, bool imported);
	bool (*config_read_player_deletion) (const char *filename, const struct config_t *config, bool imported);
	bool (*config_read_player_name) (const char *filename, const struct config_t *config, bool imported);
	void (*config_set_start_item) (const struct config_setting_t *setting);
	bool (*config_read_player_new) (const char *filename, const struct config_t *config, bool imported);
	bool (*config_read_player) (const char *filename, const struct config_t *config, bool imported);
	bool (*config_read_permission) (const char *filename, const struct config_t *config, bool imported);
	bool (*config_set_ip) (const char *type, const char *value, uint32 *out_ip, char *out_ip_str);
	bool (*config_read_inter) (const char *filename, const struct config_t *config, bool imported);
	bool (*config_read_top) (const char *filename, const struct config_t *config, bool imported);
};

#ifdef HERCULES_CORE
extern int char_name_option;
extern char char_name_letters[];
extern bool char_gm_read;
extern int autosave_interval;
extern char db_path[];
extern char char_db[256];
extern char scdata_db[256];
extern char cart_db[256];
extern char inventory_db[256];
extern char charlog_db[256];
extern char storage_db[256];
extern char interlog_db[256];
extern char skill_db[256];
extern char memo_db[256];
extern char guild_db[256];
extern char guild_alliance_db[256];
extern char guild_castle_db[256];
extern char guild_expulsion_db[256];
extern char guild_member_db[256];
extern char guild_position_db[256];
extern char guild_skill_db[256];
extern char guild_storage_db[256];
extern char party_db[256];
extern char pet_db[256];
extern char mail_db[256];
extern char auction_db[256];
extern char quest_db[256];
extern char rodex_db[256];
extern char rodex_item_db[256];
extern char homunculus_db[256];
extern char skill_homunculus_db[256];
extern char mercenary_db[256];
extern char mercenary_owner_db[256];
extern char ragsrvinfo_db[256];
extern char elemental_db[256];
extern char acc_reg_num_db[32];
extern char acc_reg_str_db[32];
extern char char_reg_str_db[32];
extern char char_reg_num_db[32];
extern char char_achievement_db[256];
extern char emotes_db[256];
extern char hotkeys_db[256];
extern char adventurer_agency_db[256];

extern int guild_exp_rate;

void char_load_defaults(void);
void char_defaults(void);
#endif // HERCULES_CORE

HPShared struct char_interface *chr;

#endif /* CHAR_CHAR_H */
