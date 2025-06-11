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

#ifndef HEADER_NETWORK_NAME_SCREEN_HPP
#define HEADER_NETWORK_NAME_SCREEN_HPP

#include "guiengine/screen.hpp"
#include "utils/cpp2011.hpp"

#include "irrString.h"

#include <functional>

namespace GUIEngine
{
    class TextBoxWidget;
    class ButtonWidget;
    class LabelWidget;
}

/**
 * \brief A screen to prompt for a temporary player name for network demo mode.
 *        The entered name is used only for the current session without creating
 *        a persistent profile. Unlike the modal dialog version, this screen 
 *        has full control over navigation and cannot be dismissed by server timeouts.
 * \ingroup states_screens
 */
class NetworkNameScreen : public GUIEngine::Screen, 
                          public GUIEngine::ScreenSingleton<NetworkNameScreen>
{
public:
    typedef std::function<void(const irr::core::stringw&)> NameEnteredCallback;
    typedef std::function<void()> CancelCallback;

private:
    friend class GUIEngine::ScreenSingleton<NetworkNameScreen>;

    GUIEngine::LabelWidget* m_title;
    GUIEngine::LabelWidget* m_subtitle;
    GUIEngine::TextBoxWidget* m_text_field;
    
    NameEnteredCallback m_name_cb;
    CancelCallback m_cancel_cb;
    
    irr::core::stringw m_previous_screen_name;

    NetworkNameScreen();

public:
    
    /** \brief implement callback from parent class GUIEngine::Screen */
    virtual void loadedFromFile() OVERRIDE;

    /** \brief implement callback from parent class GUIEngine::Screen */
    virtual void eventCallback(GUIEngine::Widget* widget, const std::string& name,
                               const int playerID) OVERRIDE;

    /** \brief implement callback from parent class GUIEngine::Screen */
    virtual void init() OVERRIDE;

    /** \brief implement callback from parent class GUIEngine::Screen */
    virtual void tearDown() OVERRIDE;

    /** \brief implement optional callback from parent class GUIEngine::Screen */
    virtual bool onEscapePressed() OVERRIDE;

    void setCallbacks(NameEnteredCallback name_cb, CancelCallback cancel_cb = nullptr);
    
    void setPreviousScreen(const irr::core::stringw& screen_name);

private:
    bool validateAndSubmit();
};

#endif