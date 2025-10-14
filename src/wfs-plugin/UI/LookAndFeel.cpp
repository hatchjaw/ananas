#include "LookAndFeel.h"

namespace ananas
{
    LookAndFeel::LookAndFeel()
    {
        auto bg{juce::Colours::ghostwhite};
        auto fg{bg.contrasting(.75f)};

        setColour(juce::ResizableWindow::backgroundColourId, bg);
    }
}
