/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-10 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#ifndef __JUCER_COMPONENTTYPEMANAGER_H_734EBF1__
#define __JUCER_COMPONENTTYPEMANAGER_H_734EBF1__

#include "../../../jucer_Headers.h"
#include "../jucer_ComponentDocument.h"
#include "../../../utility/jucer_ColourEditorComponent.h"


//==============================================================================
class ComponentTypeHandler
{
public:
    //==============================================================================
    ComponentTypeHandler (const String& name_, const String& xmlTag_, const String& memberNameRoot_);
    virtual ~ComponentTypeHandler();

    const String& getName() const               { return name; }
    const String& getXmlTag() const             { return xmlTag; }
    const String& getMemberNameRoot() const     { return memberNameRoot; }

    virtual Component* createComponent() = 0;
    virtual const Rectangle<int> getDefaultSize() = 0;

    virtual void updateComponent (ComponentDocument& document, Component* comp, const ValueTree& state);
    virtual void initialiseNewItem (ComponentDocument& document, ValueTree& state);
    virtual void createPropertyEditors (ComponentDocument& document, ValueTree& state, Array <PropertyComponent*>& props);

    Value getValue (const var::identifier& name, ValueTree& state, ComponentDocument& document) const;

    //==============================================================================
protected:
    const String name, xmlTag, memberNameRoot;

private:
    ComponentTypeHandler (const ComponentTypeHandler&);
    ComponentTypeHandler& operator= (const ComponentTypeHandler&);
};


//==============================================================================
class ComponentTypeManager  : public DeletedAtShutdown
{
public:
    //==============================================================================
    ComponentTypeManager();
    ~ComponentTypeManager();

    juce_DeclareSingleton_SingleThreaded_Minimal (ComponentTypeManager);

    //==============================================================================
    Component* createFromStoredType (ComponentDocument& document, const ValueTree& value);

    int getNumHandlers() const                                      { return handlers.size(); }
    ComponentTypeHandler* getHandler (const int index) const        { return handlers[index]; }

    ComponentTypeHandler* getHandlerFor (const String& type);
    const StringArray getTypeNames() const;


private:
    //==============================================================================
    OwnedArray <ComponentTypeHandler> handlers;
};


//==============================================================================
template <class ComponentClass>
class ComponentTypeHelper   : public ComponentTypeHandler
{
public:
    //==============================================================================
    ComponentTypeHelper (const String& name_, const String& xmlTag_, const String& memberNameRoot_)
        : ComponentTypeHandler (name_, xmlTag_, memberNameRoot_)
    {
    }

    virtual void update (ComponentDocument& document, ComponentClass* comp, const ValueTree& state) = 0;

    void updateComponent (ComponentDocument& document, Component* comp, const ValueTree& state)
    {
        ComponentTypeHandler::updateComponent (document, comp, state);

        ComponentClass* const c = dynamic_cast <ComponentClass*> (comp);
        jassert (c != 0);
        updateComponentColours (state, c);
        update (document, c, state);
    }

    virtual void initialiseNew (ComponentDocument& document, ValueTree& state) = 0;

    void initialiseNewItem (ComponentDocument& document, ValueTree& state)
    {
        ComponentTypeHandler::initialiseNewItem (document, state);
        initialiseNew (document, state);
    }

    virtual void createProperties (ComponentDocument& document, ValueTree& state, Array <PropertyComponent*>& props) = 0;

    void createPropertyEditors (ComponentDocument& document, ValueTree& state, Array <PropertyComponent*>& props)
    {
        ComponentTypeHandler::createPropertyEditors (document, state, props);
        createProperties (document, state, props);
    }

protected:
    //==============================================================================
    void addTooltipProperty (ComponentDocument& document, ValueTree& state, Array <PropertyComponent*>& props)
    {
        props.add (new TextPropertyComponent (getValue (ComponentDocument::compTooltipProperty, state, document),
                                              "Tooltip", 4096, false));
    }

    void addFocusOrderProperty (ComponentDocument& document, ValueTree& state, Array <PropertyComponent*>& props)
    {
        props.add (new TextPropertyComponent (Value (new IntegerValueSource (getValue (ComponentDocument::compFocusOrderProperty,
                                                                                       state, document))),
                                              "Focus Order", 10, false));
    }

    //==============================================================================
    struct EditableColour
    {
        int colourId;
        String name, propertyName;

        PropertyComponent* createProperty (ComponentTypeHelper& type, ComponentDocument& document, ValueTree& state)
        {
            return new ColourPropertyComponent (document, name, type.getValue (propertyName, state, document),
                                                LookAndFeel::getDefaultLookAndFeel().findColour (colourId), true);
        }

        void updateComponent (const ValueTree& state, Component* component)
        {
            const String colour (state [propertyName].toString());
            if (colour.isNotEmpty())
                component->setColour (colourId, Colour::fromString (colour));
            else
                component->removeColour (colourId);
        }
    };

    Array <EditableColour> editableColours;

    void addEditableColour (int colourId, const String& displayName, const String& propertyName)
    {
        EditableColour ec;
        ec.colourId = colourId;
        ec.name = displayName;
        ec.propertyName = propertyName;
        editableColours.add (ec);
    }

    void addEditableColourProperties (ComponentDocument& document, ValueTree& state, Array <PropertyComponent*>& props)
    {
        for (int i = 0; i < editableColours.size(); ++i)
            props.add (editableColours.getReference(i).createProperty (*this, document, state));
    }

    void updateComponentColours (const ValueTree& state, Component* component)
    {
        for (int i = 0; i < editableColours.size(); ++i)
            editableColours.getReference(i).updateComponent (state, component);
    }
};


#endif  // __JUCER_COMPONENTTYPEMANAGER_H_734EBF1__