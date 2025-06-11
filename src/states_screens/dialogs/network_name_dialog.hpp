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

#ifndef HEADER_NETWORK_NAME_DIALOG_HPP
#define HEADER_NETWORK_NAME_DIALOG_HPP

#include "guiengine/modaldialog.hpp"
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
 * \brief A dialog to prompt for a temporary player name for network demo mode.
 *        The entered name is used only for the current session without creating
 *        a persistent profile.
 * \ingroup states_screens
 */
class NetworkNameDialog : public GUIEngine::ModalDialog
{
public:
    typedef std::function<void(const irr::core::stringw&)> NameEnteredCallback;
    typedef std::function<void()> CancelCallback;

private:
    GUIEngine::LabelWidget* m_title;
    GUIEngine::LabelWidget* m_subtitle;
    GUIEngine::TextBoxWidget* m_text_field;
    
    NameEnteredCallback m_name_cb;
    CancelCallback m_cancel_cb;
    
    bool m_self_destroy;

public:
    NetworkNameDialog(NameEnteredCallback name_cb, CancelCallback cancel_cb = nullptr);
    
    virtual ~NetworkNameDialog();
    
    virtual void onEnterPressedInternal() OVERRIDE;
    
    virtual void onUpdate(float dt) OVERRIDE;
    
    GUIEngine::EventPropagation processEvent(const std::string& eventSource) OVERRIDE;
    
    GUIEngine::TextBoxWidget* getTextField() const { return m_text_field; }

private:
    bool validateAndSubmit();
};

#endif