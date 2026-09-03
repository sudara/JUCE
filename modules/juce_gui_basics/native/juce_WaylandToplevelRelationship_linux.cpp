/*
  ==============================================================================

   This file is part of the JUCE framework.
   Copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

   By downloading, installing, or using the JUCE framework, or combining the
   JUCE framework with any other source code, object code, content or any other
   copyrightable work, you agree to the terms of the JUCE End User Licence
   Agreement, and all incorporated terms including the JUCE Privacy Policy and
   the JUCE Website Terms of Service, as applicable, which will bind you. If you
   do not agree to the terms of these agreements, we will not license the JUCE
   framework to you, and you must discontinue the installation or download
   process and cease use of the JUCE framework.

   JUCE End User Licence Agreement: https://juce.com/legal/juce-9-licence/
   JUCE Privacy Policy: https://juce.com/juce-privacy-policy
   JUCE Website Terms of Service: https://juce.com/juce-website-terms/

   Or:

   You may also use this code under the terms of the AGPLv3:
   https://www.gnu.org/licenses/agpl-3.0.en.html

   THE JUCE FRAMEWORK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL
   WARRANTIES, WHETHER EXPRESSED OR IMPLIED, INCLUDING WARRANTY OF
   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

bool WaylandToplevelRelationship::updateParent (xdg_toplevel& child, xdg_toplevel* newParent)
{
    if (! parent.has_value() && newParent == nullptr)
    {
        parent = nullptr;
        return false;
    }

    if (parent == newParent)
        return false;

    WaylandProtocol::xdgToplevelSetParent (&child, newParent);
    parent = newParent;
    return true;
}

void WaylandToplevelRelationship::invalidateParent()
{
    parent.reset();
}

bool WaylandToplevelRelationship::updateDialog (xdg_toplevel& child,
                                                xdg_wm_dialog_v1* manager,
                                                bool isModal)
{
    if (manager == nullptr)
    {
        const auto hadDialog = dialog != nullptr;
        dialog.reset();
        modalState.reset();
        return hadDialog;
    }

    auto changed = false;

    if (dialog == nullptr)
    {
        dialog.reset (WaylandProtocol::xdgWmDialogV1GetXdgDialog (manager, &child));
        modalState.reset();
        changed = dialog != nullptr;
    }

    if (dialog == nullptr)
        return false;

    if (! modalState.has_value() && ! isModal)
    {
        modalState = false;
        return changed;
    }

    if (modalState == isModal)
        return changed;

    if (isModal)
        WaylandProtocol::xdgDialogV1SetModal (dialog.get());
    else
        WaylandProtocol::xdgDialogV1UnsetModal (dialog.get());

    modalState = isModal;
    return true;
}

} // namespace juce
