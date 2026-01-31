#ifndef LOOKANDFEEL_H
#define LOOKANDFEEL_H

#include <juce_gui_basics/juce_gui_basics.h>

namespace ananas
{
    class LookAndFeel final : public juce::LookAndFeel_V4
    {
    public:
        LookAndFeel();

        int getTabButtonBestWidth(juce::TabBarButton &, int tabDepth) override;

        void setTotalWidth(int width);

        void setNumberOfTabs(int numTabs);

    private:
        int totalWidth;
        int totalNumTabs;
    };
}


#endif //LOOKANDFEEL_H
