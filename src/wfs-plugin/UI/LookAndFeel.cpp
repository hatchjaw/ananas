#include "LookAndFeel.h"

namespace ananas
{
    LookAndFeel::LookAndFeel()
    {
        auto bg{juce::Colours::ghostwhite};
        auto fg{bg.contrasting(.75f)};

        setColour(juce::ResizableWindow::backgroundColourId, bg);
    }

    int LookAndFeel::getTabButtonBestWidth(juce::TabBarButton &tabBarButton, const int tabDepth)
    {
        return totalNumTabs > 0 ? totalWidth / totalNumTabs : LookAndFeel_V4::getTabButtonBestWidth(tabBarButton, tabDepth);
    }

    void LookAndFeel::setTotalWidth(int width)
    {
        totalWidth = width;
    }

    void LookAndFeel::setNumberOfTabs(int numTabs)
    {
        totalNumTabs = numTabs;
    }
}
