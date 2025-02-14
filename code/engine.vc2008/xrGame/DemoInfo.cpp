#include "stdafx.h"
#include "level.h"
#include "DemoInfo.h"
#include "../xrCore/stream_reader.h"
#include "object_broker.h"
#include "game_cl_base.h"

LPCSTR GameTypeToString(EGameIDs gt, bool bShort);

u32 const demo_player_info::demo_info_max_size = DEMOSTRING_MAX_SIZE + 80;

demo_player_info::demo_player_info()
{
}

demo_player_info::~demo_player_info()
{
}

void demo_player_info::read_from_file(CStreamReader* file_to_read)
{
	file_to_read->r_stringZ	(m_name);
	m_frags					= file_to_read->r_s16();
	m_deaths				= file_to_read->r_s16();
	m_artefacts				= file_to_read->r_u16();
	m_spots					= file_to_read->r_s16();
	m_rank					= file_to_read->r_u8();
}

void demo_player_info::write_to_file(IWriter* file_to_write) const
{
	file_to_write->w_stringZ(m_name);
	file_to_write->w_s16	(m_frags);
	file_to_write->w_s16	(m_deaths);
	file_to_write->w_u16	(m_artefacts);
	file_to_write->w_s16	(m_spots);
	file_to_write->w_u8		(m_rank);
}

u32 const demo_info::max_demo_info_size = 
	(demo_player_info::demo_info_max_size) +
	(DEMOSTRING_MAX_SIZE * 5) + sizeof(u32);

demo_info::demo_info()
{
}

demo_info::~demo_info()
{
	delete_data				(m_players);
}

void demo_info::read_from_file(CStreamReader* file_to_read)
{
	size_t old_pos = file_to_read->tell();
	file_to_read->r_stringZ	(m_map_name);
	file_to_read->r_stringZ	(m_map_version);
	file_to_read->r_stringZ	(m_game_type);
	file_to_read->r_stringZ	(m_game_score);
	file_to_read->r_stringZ	(m_author_name);
	R_ASSERT(file_to_read->tell() - old_pos <= (DEMOSTRING_MAX_SIZE * 5));
	
	m_players_count			= file_to_read->r_u32();
	
	delete_data				(m_players);

	m_players.reserve		(m_players_count);
	for (u32 i = 0; i < m_players_count; ++i)
	{
		demo_player_info* new_player = xr_new<demo_player_info>();
		
		new_player->read_from_file	(file_to_read);
		m_players.push_back			(new_player);
	}
}
void demo_info::write_to_file(IWriter* file_to_write) const
{
	file_to_write->w_stringZ	(m_map_name);
	file_to_write->w_stringZ	(m_map_version);
	file_to_write->w_stringZ	(m_game_type);
	file_to_write->w_stringZ	(m_game_score);
	file_to_write->w_stringZ	(m_author_name);
	
	file_to_write->w_u32		((u32)m_players.size());
	for (players_coll_t::const_iterator i = m_players.begin(),
		ie = m_players.end(); i != ie; ++i)
	{
		(*i)->write_to_file(file_to_write);
	}
}
void demo_info::sort_players(sorting_less_comparator sorting_comparator)
{
	std::sort(m_players.begin(), m_players.end(), sorting_comparator);
}

void demo_info::load_from_game()
{
	m_map_name = Level().name();
	m_map_version = Level().version();

	m_game_type = GameTypeToString(Game().Type(), true);
	m_game_score = "";
	m_author_name = "unknown";

	u32	pcount = Game().GetPlayersCount();

	delete_data(m_players);
	m_players.reserve(pcount);
}

demo_player_info const * demo_info::get_player(u32 player_index) const
{
	R_ASSERT(player_index < m_players.size());
	return m_players[player_index];
}


using namespace luabind;

#pragma optimize("s",on)
void demo_player_info::script_register(lua_State *L)
{
	module(L)
	[
		class_<demo_player_info>("demo_player_info")
			.def("get_name",		&demo_player_info::get_name)
			.def("get_frags",		&demo_player_info::get_frags)
			.def("get_deaths",		&demo_player_info::get_deaths)
			.def("get_artefacts",	&demo_player_info::get_artefacts)
			.def("get_spots",		&demo_player_info::get_spots)
			.def("get_team",		&demo_player_info::get_team)
			.def("get_rank",		&demo_player_info::get_rank)
	];
}

void demo_info::script_register(lua_State *L)
{
	module(L)
	[
		class_<demo_info>("demo_info")
			.def("get_map_name",		&demo_info::get_map_name)
			.def("get_map_version",		&demo_info::get_map_version)
			.def("get_game_type",		&demo_info::get_game_type)
			.def("get_game_score",		&demo_info::get_game_score)
			.def("get_author_name",		&demo_info::get_author_name)
			.def("get_players_count",	&demo_info::get_players_count)
			.def("get_player",			&demo_info::get_player)
	];
}
