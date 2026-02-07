#include "LookAndFeel.h"

namespace ananas::UI {
    LookAndFeel::LookAndFeel()
    {
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colours::ghostwhite);

        setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::mediumvioletred);
    }

    int LookAndFeel::getTabButtonBestWidth(juce::TabBarButton &tabBarButton, const int tabDepth)
    {
        return totalNumTabs > 0 ? totalWidth / totalNumTabs : LookAndFeel_V4::getTabButtonBestWidth(tabBarButton, tabDepth);
    }

    void LookAndFeel::setTotalWidth(const int width)
    {
        totalWidth = width;
    }

    void LookAndFeel::setNumberOfTabs(const int numTabs)
    {
        totalNumTabs = numTabs;
    }
}
