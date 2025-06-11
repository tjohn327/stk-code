//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2025 SuperTuxKart-Team
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

#include "states_screens/network_name_screen.hpp"

#include "guiengine/engine.hpp"
#include "guiengine/widgets/button_widget.hpp"
#include "guiengine/widgets/label_widget.hpp"
#include "guiengine/widgets/text_box_widget.hpp"
#include "states_screens/state_manager.hpp"
#include "states_screens/main_menu_screen.hpp"
#include "utils/string_utils.hpp"
#include "utils/translation.hpp"

using namespace GUIEngine;
using namespace irr::core;


// -----------------------------------------------------------------------------
NetworkNameScreen::NetworkNameScreen() : Screen("network_name_screen.stkgui")
{
}

// -----------------------------------------------------------------------------
void NetworkNameScreen::loadedFromFile()
{
    m_text_field = getWidget<TextBoxWidget>("textfield");
    assert(m_text_field != NULL);

    m_title = getWidget<LabelWidget>("title");
    assert(m_title != NULL);
    
    m_subtitle = getWidget<LabelWidget>("subtitle");
    assert(m_subtitle != NULL);
}

// -----------------------------------------------------------------------------
void NetworkNameScreen::init()
{
    Screen::init();
    
    // Set focus to the text field
    m_text_field->setFocusForPlayer(PLAYER_ID_GAME_MASTER);
    m_text_field->setText(L"");
    
    // Reset title text in case it was changed for error display
    m_title->setText(_("Enter Your Name"), false);
}

// -----------------------------------------------------------------------------
void NetworkNameScreen::tearDown()
{
    Screen::tearDown();
}

// -----------------------------------------------------------------------------
void NetworkNameScreen::eventCallback(GUIEngine::Widget* widget, const std::string& name,
                                       const int playerID)
{
    if (name == "cancel")
    {
        if (m_cancel_cb)
            m_cancel_cb();
        
        // Go back to previous screen or main menu
        if (!m_previous_screen_name.empty())
            StateManager::get()->escapePressed();
        else
            StateManager::get()->replaceTopMostScreen(MainMenuScreen::getInstance());
    }
    else if (name == "ok")
    {
        validateAndSubmit();
    }
}

// -----------------------------------------------------------------------------
bool NetworkNameScreen::onEscapePressed()
{
    // Same as cancel button
    if (m_cancel_cb)
        m_cancel_cb();
    
    return true; // Handle the escape
}

// -----------------------------------------------------------------------------
bool NetworkNameScreen::validateAndSubmit()
{
    stringw name = m_text_field->getText().trim();
    
    // Validate name
    if (name.empty())
    {
        // Show error message by changing title text temporarily
        m_title->setText(_("Please enter a name"), false);
        return false;
    }
    
    // Limit name length to 30 characters (consistent with server)
    if (name.size() > 30)
    {
        name = name.subString(0, 30);
        m_text_field->setText(name);
    }
    
    // Call the callback with the validated name
    if (m_name_cb)
        m_name_cb(name);
    
    // Go back to previous screen or main menu
    if (!m_previous_screen_name.empty())
        StateManager::get()->escapePressed();
    else
        StateManager::get()->replaceTopMostScreen(MainMenuScreen::getInstance());
    
    return true;
}

// -----------------------------------------------------------------------------
void NetworkNameScreen::setCallbacks(NameEnteredCallback name_cb, CancelCallback cancel_cb)
{
    m_name_cb = name_cb;
    m_cancel_cb = cancel_cb;
}

// -----------------------------------------------------------------------------
void NetworkNameScreen::setPreviousScreen(const irr::core::stringw& screen_name)
{
    m_previous_screen_name = screen_name;
}