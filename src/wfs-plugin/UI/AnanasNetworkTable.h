#ifndef ANANASNETWORKTABLE_H
#define ANANASNETWORKTABLE_H

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Utils.h"

namespace ananas
{
    class AnanasNetworkTable : public juce::Component,
                               public juce::TableListBoxModel
    {
    protected:
        void addColumn(const WFS::TableColumns::ColumnHeader &h) const;

        juce::TableListBox table;
    };
} // ananas

#endif //ANANASNETWORKTABLE_H
