/*
Minetest
Copyright (C) 2013 celeron55, Perttu Ahola <celeron55@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3.0 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "settings.h"
#include "config.h"
#include "mapgen/mapgen.h" // Mapgen::setDefaultSettings

void set_default_settings()
{
	Settings *settings = Settings::getLayer(SL_DEFAULTS);
	if (settings == nullptr)
		settings = Settings::createLayer(SL_DEFAULTS);

	// General
	settings->setDefault("language", "");
	settings->setDefault("name", "");
	settings->setDefault("bind_address", "");
	settings->setDefault("serverlist_url", "https://servers.luanti.org");

	// HTTP
	settings->setDefault("curl_timeout", "5000");
	settings->setDefault("curl_parallel_limit", "8");
	settings->setDefault("curl_verify_cert", "true");

	// Server
	settings->setDefault("texture_path", "");
	settings->setDefault("recent_chat_messages", "10");
	settings->setDefault("compat_player_model", "character.b3d,3d_armor_character.b3d,skinsdb_3d_armor_character_5.b3d");
	settings->setDefault("compat_send_original_model", "true");
	settings->setDefault("disable_texture_packs", "false");
	settings->setDefault("disable_escape_sequences", "false");
	settings->setDefault("strip_color_codes", "true");
#if USE_PROMETHEUS
	settings->setDefault("prometheus_listener_address", "127.0.0.1:30000");
#endif

	// Network
	settings->setDefault("enable_ipv6", "true");
	settings->setDefault("ipv6_server", "false");
	settings->setDefault("max_packets_per_iteration","1024");
	settings->setDefault("port", "40000");
	settings->setDefault("enable_protocol_compat", "true");
	settings->setDefault("strict_protocol_version_checking", "false");
	settings->setDefault("player_transfer_distance", "0");
	settings->setDefault("max_simultaneous_block_sends_per_client", "40");
	settings->setDefault("time_send_interval", "5");

	settings->setDefault("default_game", "default");
	settings->setDefault("motd", "");
	settings->setDefault("max_users", "15");
	settings->setDefault("creative_mode", "false");
	settings->setDefault("enable_damage", "true");
	settings->setDefault("default_password", "");
	settings->setDefault("default_privs", "interact, shout");
	settings->setDefault("enable_pvp", "true");
	settings->setDefault("enable_mod_channels", "true");
	settings->setDefault("disallow_empty_password", "false");
	settings->setDefault("disable_anticheat", "false");
	settings->setDefault("enable_rollback_recording", "false");
	settings->setDefault("deprecated_lua_api_handling", "log");

	settings->setDefault("kick_msg_shutdown", "Server shutting down.");
	settings->setDefault("kick_msg_crash", "This server has experienced an internal error. You will now be disconnected.");
	settings->setDefault("ask_reconnect_on_crash", "false");

	settings->setDefault("chat_message_format", "@name: @message");
	settings->setDefault("profiler_print_interval", "0");
	settings->setDefault("active_object_send_range_blocks", "8");
	settings->setDefault("active_block_range", "4");
	//settings->setDefault("max_simultaneous_block_sends_per_client", "1");
	settings->setDefault("max_block_send_distance", "12");
	settings->setDefault("block_send_optimize_distance", "4");
	settings->setDefault("server_side_occlusion_culling", "true");
	settings->setDefault("csm_restriction_flags", "60");
	settings->setDefault("csm_restriction_noderange", "0");
	settings->setDefault("max_clearobjects_extra_loaded_blocks", "4096");
	settings->setDefault("time_speed", "72");
	settings->setDefault("world_start_time", "6125");
	settings->setDefault("server_unload_unused_data_timeout", "29");
	settings->setDefault("max_objects_per_block", "64");
	settings->setDefault("server_map_save_interval", "5.3");
	settings->setDefault("chat_message_max_size", "500");
	settings->setDefault("chat_message_limit_per_10sec", "5.0");
	settings->setDefault("chat_message_limit_trigger_kick", "50");
	settings->setDefault("sqlite_synchronous", "2");
	settings->setDefault("map_compression_level_disk", "3");
	settings->setDefault("map_compression_level_net", "-1");
	settings->setDefault("full_block_send_enable_min_time_from_building", "2.0");
	settings->setDefault("dedicated_server_step", "0.09");
	settings->setDefault("active_block_mgmt_interval", "2.0");
	settings->setDefault("abm_interval", "1.0");
	settings->setDefault("abm_time_budget", "0.2");
	settings->setDefault("nodetimer_interval", "0.2");
	settings->setDefault("ignore_world_load_errors", "false");
	settings->setDefault("remote_media", "");
	settings->setDefault("debug_log_level", "warning");
	settings->setDefault("debug_log_size_max", "50");
	settings->setDefault("chat_log_level", "error");
	settings->setDefault("emergequeue_limit_total", "1024");
	settings->setDefault("emergequeue_limit_diskonly", "128");
	settings->setDefault("emergequeue_limit_generate", "128");
	settings->setDefault("num_emerge_threads", "1");
	settings->setDefault("log_mod_memory_usage_on_load", "false");
	settings->setDefault("secure.enable_security", "true");
	settings->setDefault("secure.trusted_mods", "");
	settings->setDefault("secure.http_mods", "");

	// Physics
	settings->setDefault("movement_acceleration_default", "3");
	settings->setDefault("movement_acceleration_air", "2");
	settings->setDefault("movement_acceleration_fast", "20");
	settings->setDefault("movement_speed_walk", "4");
	settings->setDefault("movement_speed_crouch", "1.35");
	settings->setDefault("movement_speed_fast", "20");
	settings->setDefault("movement_speed_climb", "3");
	settings->setDefault("movement_speed_jump", "6.5");
	settings->setDefault("movement_liquid_fluidity", "1");
	settings->setDefault("movement_liquid_fluidity_smooth", "0.5");
	settings->setDefault("movement_liquid_sink", "20");
	settings->setDefault("movement_gravity", "9.81");

	// Liquids
	settings->setDefault("liquid_loop_max", "100000");
	settings->setDefault("liquid_queue_purge_time", "0");
	settings->setDefault("liquid_update", "1.0");

	// Mapgen
	settings->setDefault("mg_name", "v7p");
	settings->setDefault("water_level", "1");
	settings->setDefault("mapgen_limit", "31000");
	settings->setDefault("chunksize", "5");
	settings->setDefault("fixed_map_seed", "");
	settings->setDefault("max_block_generate_distance", "10");
	settings->setDefault("enable_mapgen_debug_info", "false");
	Mapgen::setDefaultSettings(settings);

	// Server list announcing
	settings->setDefault("server_announce", "false");
	settings->setDefault("server_announce_send_players", "false");
	settings->setDefault("server_url", "");
	settings->setDefault("server_address", "");
	settings->setDefault("server_name", "");
	settings->setDefault("server_description", "");

	settings->setDefault("enable_console", "false");

}
