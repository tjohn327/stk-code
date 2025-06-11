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

#include "states_screens/dialogs/network_name_dialog.hpp"

#include "guiengine/engine.hpp"
#include "guiengine/widgets/icon_button_widget.hpp"
#include "guiengine/widgets/label_widget.hpp"
#include "guiengine/widgets/ribbon_widget.hpp"
#include "guiengine/widgets/text_box_widget.hpp"
#include "states_screens/state_manager.hpp"
#include "utils/string_utils.hpp"
#include "utils/translation.hpp"

#include <IGUIEnvironment.h>

using namespace GUIEngine;
using namespace irr::core;

// -----------------------------------------------------------------------------
NetworkNameDialog::NetworkNameDialog(NameEnteredCallback name_cb, CancelCallback cancel_cb)
                  : ModalDialog(0.95f, 0.6f, GUIEngine::MODAL_DIALOG_LOCATION_CENTER),
                    m_name_cb(name_cb), m_cancel_cb(cancel_cb), m_self_destroy(false)
{
    loadFromFile("network_name_dialog.stkgui");

    m_text_field = getWidget<TextBoxWidget>("textfield");
    assert(m_text_field != NULL);
    m_text_field->setFocusForPlayer(PLAYER_ID_GAME_MASTER);

    m_title = getWidget<LabelWidget>("title");
    assert(m_title != NULL);
    
    m_subtitle = getWidget<LabelWidget>("subtitle");
    assert(m_subtitle != NULL);
    
    // Set default placeholder text
    m_text_field->setText(L"");
}

// -----------------------------------------------------------------------------
NetworkNameDialog::~NetworkNameDialog()
{
    m_text_field->getIrrlichtElement()->remove();
    m_text_field->clearListeners();
}

// -----------------------------------------------------------------------------
GUIEngine::EventPropagation NetworkNameDialog::processEvent(const std::string& eventSource)
{ 
    GUIEngine::RibbonWidget* buttons_ribbon =
        getWidget<GUIEngine::RibbonWidget>("buttons");
    
    if(eventSource == "buttons")
    {
        const std::string& button =
            buttons_ribbon->getSelectionIDString(PLAYER_ID_GAME_MASTER);

        if (button == "cancel")
        {
            if (m_cancel_cb)
                m_cancel_cb();
            dismiss();
            return GUIEngine::EVENT_BLOCK;
        }
        else if (button == "ok")
        {
            if (!m_self_destroy && validateAndSubmit())
                m_self_destroy = true;
            return GUIEngine::EVENT_BLOCK;
        }
    }
    return GUIEngine::EVENT_LET;
}

// -----------------------------------------------------------------------------
void NetworkNameDialog::onEnterPressedInternal()
{
    // Cancel button pressed
    IconButtonWidget* cancel_button = getWidget<IconButtonWidget>("cancel");
    if (GUIEngine::isFocusedForPlayer(cancel_button, PLAYER_ID_GAME_MASTER))
    {
        if (m_cancel_cb)
            m_cancel_cb();
        dismiss();
        return;
    }

    // Enter pressed in text field or OK button focused
    if (!m_self_destroy && validateAndSubmit())
        m_self_destroy = true;
}

// -----------------------------------------------------------------------------
bool NetworkNameDialog::validateAndSubmit()
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
    
    return true;
}

// -----------------------------------------------------------------------------
void NetworkNameDialog::onUpdate(float dt)
{
    // It's unsafe to delete from inside the event handler so we do it here
    if (m_self_destroy)
    {
        stringw name = m_text_field->getText().trim();
        
        // Ensure name is not empty and within limits
        if (name.empty())
            name = L"Player";
        else if (name.size() > 30)
            name = name.subString(0, 30);

        // irrLicht is too stupid to remove focus from deleted widgets
        // so do it by hand
        GUIEngine::getGUIEnv()
            ->removeFocus(m_text_field->getIrrlichtElement());
        GUIEngine::getGUIEnv()->removeFocus(m_irrlicht_window);

        // We will destroy the dialog before notifying the callback is run,
        // but in order not to crash we must make a local copy of the callback
        // otherwise we will crash
        NameEnteredCallback name_cb = m_name_cb;
        ModalDialog::dismiss();
        
        if (name_cb)
            name_cb(name);
    }
}