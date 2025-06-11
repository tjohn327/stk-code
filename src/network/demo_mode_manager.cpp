//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2024 SuperTuxKart-Team
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "network/demo_mode_manager.hpp"

#include "config/user_config.hpp"
#include "guiengine/engine.hpp"
#include "guiengine/modaldialog.hpp"
#include "network/protocols/client_lobby.hpp"
#include "network/server.hpp"
#include "network/stk_host.hpp"
#include "states_screens/network_name_screen.hpp"
#include "states_screens/online/networking_lobby.hpp"
#include "states_screens/main_menu_screen.hpp"
#include "states_screens/state_manager.hpp"
#include "utils/log.hpp"
#include "utils/string_utils.hpp"

DemoModeManager* DemoModeManager::m_instance = nullptr;

// -----------------------------------------------------------------------------
DemoModeManager* DemoModeManager::get()
{
    if (!m_instance)
        m_instance = new DemoModeManager();
    return m_instance;
}

// -----------------------------------------------------------------------------
void DemoModeManager::destroy()
{
    delete m_instance;
    m_instance = nullptr;
}

// -----------------------------------------------------------------------------
void DemoModeManager::handleRaceEndContinue()
{
    // Force clear demo name for next race if in demo mode
    if (UserConfigParams::m_network_demo_mode && !g_network_demo_current_name.empty())
    {
        Log::info("DemoModeManager", "Demo mode: clearing demo name '%s' to force new prompt", 
                  g_network_demo_current_name.c_str());
        g_network_demo_current_name.clear();
    }
    
    // Only handle if we're in demo mode and need a name
    if (!UserConfigParams::m_network_demo_mode || 
        !g_network_demo_current_name.empty() ||
        GUIEngine::ModalDialog::isADialogActive())
    {
        return;
    }
    
    Log::info("DemoModeManager", "Starting demo mode flow: race end -> disconnect -> name dialog -> reconnect");
    
    m_demo_flow_active = true;
    
    // Clear any existing demo name to force new prompt
    g_network_demo_current_name.clear();
    
    // Store current server info for reconnection
    auto client_lobby = LobbyProtocol::get<ClientLobby>();
    if (client_lobby)
    {
        m_stored_server = client_lobby->getJoinedServer();
        if (m_stored_server)
        {
            Log::info("DemoModeManager", "Stored server for reconnection: %s", 
                      StringUtils::wideToUtf8(m_stored_server->getName()).c_str());
        }
    }
    
    // Disconnect from server
    Log::info("DemoModeManager", "Disconnecting from server");
    STKHost::get()->disconnectAllPeers(false);
    STKHost::get()->requestShutdown();
    
    // Set up name screen callbacks and show it
    NetworkNameScreen::getInstance()->setCallbacks(
        [this](const core::stringw& name) { 
            handleNameEntered(StringUtils::wideToUtf8(name)); 
        },
        [this]() { 
            handleNameCancelled(); 
        }
    );
    
    Log::info("DemoModeManager", "Showing name dialog");
    StateManager::get()->pushScreen(NetworkNameScreen::getInstance());
}

// -----------------------------------------------------------------------------
void DemoModeManager::handleNameEntered(const std::string& name)
{
    Log::info("DemoModeManager", "Demo name entered: %s", name.c_str());
    
    // Store the demo name
    g_network_demo_current_name = name;
    
    // Reconnect to the stored server
    if (m_stored_server)
    {
        Log::info("DemoModeManager", "Reconnecting to server: %s", 
                  StringUtils::wideToUtf8(m_stored_server->getName()).c_str());
        
        // Set the server for networking lobby and transition to it
        NetworkingLobby::getInstance()->setJoinedServer(m_stored_server);
        
        // Go to networking lobby which will handle the connection
        StateManager::get()->replaceTopMostScreen(NetworkingLobby::getInstance());
    }
    else
    {
        Log::warn("DemoModeManager", "No stored server found - returning to main menu");
        StateManager::get()->resetAndGoToScreen(MainMenuScreen::getInstance());
    }
    
    m_demo_flow_active = false;
}

// -----------------------------------------------------------------------------
void DemoModeManager::handleNameCancelled()
{
    Log::info("DemoModeManager", "Demo name entry cancelled - returning to main menu");
    
    // Clear demo name and return to main menu
    g_network_demo_current_name.clear();
    StateManager::get()->resetAndGoToScreen(MainMenuScreen::getInstance());
    
    m_demo_flow_active = false;
}

// -----------------------------------------------------------------------------
void DemoModeManager::reset()
{
    Log::info("DemoModeManager", "Resetting demo mode state");
    m_stored_server = nullptr;
    m_demo_flow_active = false;
}