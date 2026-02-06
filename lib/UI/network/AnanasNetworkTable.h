#ifndef ANANASNETWORKTABLE_H
#define ANANASNETWORKTABLE_H

#include <juce_gui_basics/juce_gui_basics.h>
#include "../AnanasUIUtils.h"

namespace ananas::UI
{
    class AnanasNetworkTable : public juce::Component,
                               public juce::TableListBoxModel
    {
    protected:
        void addColumn(const TableColumns::ColumnHeader &h) const;

        juce::TableListBox table;
    };
}

#endif //ANANASNETWORKTABLE_H
