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

#ifndef HEADER_DEMO_MODE_MANAGER_HPP
#define HEADER_DEMO_MODE_MANAGER_HPP

#include <memory>
#include <functional>

class Server;

/**
 * \brief Manages demo mode functionality for network races
 * 
 * Handles the flow of: race ends -> disconnect -> show name dialog -> reconnect
 * This keeps demo mode logic separate from GUI components.
 */
class DemoModeManager
{
private:
    static DemoModeManager* m_instance;
    
    std::shared_ptr<Server> m_stored_server;
    bool m_demo_flow_active;
    
    DemoModeManager() : m_stored_server(nullptr), m_demo_flow_active(false) {}
    
public:
    static DemoModeManager* get();
    static void destroy();
    
    /**
     * Called when continue button is pressed after race ends in demo mode
     * Initiates the disconnect -> name dialog -> reconnect flow
     */
    void handleRaceEndContinue();
    
    /**
     * Called when user enters a name in the demo mode name dialog
     * Completes the reconnection process
     */
    void handleNameEntered(const std::string& name);
    
    /**
     * Called when user cancels the name dialog
     * Returns to main menu
     */
    void handleNameCancelled();
    
    /**
     * Check if demo mode flow is currently active
     */
    bool isDemoFlowActive() const { return m_demo_flow_active; }
    
    /**
     * Reset demo mode state (used when manually disconnecting)
     */
    void reset();
};

#endif // HEADER_DEMO_MODE_MANAGER_HPP