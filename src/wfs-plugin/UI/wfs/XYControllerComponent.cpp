#include "XYControllerComponent.h"

#include "../../Utils.h"

namespace ananas::WFS
{
    XYControllerComponent::XYControllerComponent(const uint numNodesToCreate, juce::AudioProcessorValueTreeState &apvts)
        : state(apvts)
    {
        apvts.addParameterListener(Params::SpeakerSpacing.id, this);
        calculateGridSpacingX(apvts.getRawParameterValue(Params::SpeakerSpacing.id)->load());

        for (uint n{0}; n < numNodesToCreate; ++n) {
            // Add a node for each sound source
            const auto node{nodes.add(new Node(n))};
            addAndMakeVisible(node);
            node->setBroughtToFrontOnMouseClick(true);

            // Add a parameter attachment
            attachments.add(new Attachment(
                apvts,
                Params::getSourcePositionParamID(n, SourcePositionAxis::X),
                Params::getSourcePositionParamID(n, SourcePositionAxis::Y),
                *node
            ));
        }
    }

    void XYControllerComponent::paint(juce::Graphics &g)
    {
        g.fillAll(juce::Colour{0.f, 0.f, 0.f, .1f});

        g.setColour(Constants::UI::XYControllerGridlineColour);
        const auto left{getBounds().toFloat().getWidth()};
        const auto halfHeight{getHeight() / 2};

        // Draw virtual source y grid lines, one per metre.
        const auto posUnit{halfHeight / Constants::MaxYMetres};
        for (int y{0}; y < Constants::MaxYMetres; ++y) {
            g.drawHorizontalLine(halfHeight - y * posUnit, 0.f, left);
        }

        // Draw focused source y grid lines, one per metre.
        const auto negUnit{halfHeight / Constants::MinYMetres};
        for (int y{-1}; y > Constants::MinYMetres; --y) {
            g.drawHorizontalLine(halfHeight + y * negUnit, 0.f, left);
        }

        if (xGridSpacing > 0) {
            // Draw positive x grid lines, one per metre
            for (int x{getWidth() / 2}; x < getWidth(); x += xGridSpacing) {
                g.drawVerticalLine(x, 0.f, getHeight());
            }

            // Draw negative x grid lines
            for (int x{getWidth() / 2 - xGridSpacing}; x > 0; x -= xGridSpacing) {
                g.drawVerticalLine(x, 0.f, getHeight());
            }
        }
    }

    void XYControllerComponent::resized()
    {
        for (const auto &node: nodes) {
            node->setBounds();
        }

        calculateGridSpacingX(state.getRawParameterValue(Params::SpeakerSpacing.id)->load());
    }

    void XYControllerComponent::calculateGridSpacingX(const float newValue)
    {
        // x grid spacing is speaker spacing times twice the number of
        // modules...
        const auto halfArrayWidth{newValue * Constants::NumModules};
        const auto halfXYWidth{getWidth() / 2};
        xGridSpacing = halfXYWidth / halfArrayWidth;
    }

    void XYControllerComponent::parameterChanged(const juce::String &parameterID, const float newValue)
    {
        if (parameterID == Params::SpeakerSpacing.id) {
            calculateGridSpacingX(newValue);
            repaint();
        }
    }

    //==========================================================================

    XYControllerComponent::Node::Node(const uint idx) : index(idx)
    {
    }

    void XYControllerComponent::Node::paint(juce::Graphics &g)
    {
        const auto colour{
            juce::Colours::black
            .withSaturation(.5f)
            .withAlpha(.5f)
        };
        g.setColour(colour);
        g.fillEllipse(getLocalBounds().toFloat());
        g.setColour(colour.darker(.25));
        g.drawEllipse(getLocalBounds().withSizeKeepingCentre(getWidth() - 2, getHeight() - 2).toFloat(), 2.f);
        g.setColour(juce::Colours::white);
        g.setFont(20);
        g.drawText(juce::String(index + 1), getLocalBounds(), juce::Justification::centred);
    }

    void XYControllerComponent::Node::mouseDown(const juce::MouseEvent &event)
    {
        ignoreUnused(event);
        currentDrag.reset();
        currentDrag = std::make_unique<ScopedDragNotification>(*this);
    }

    void XYControllerComponent::Node::mouseDrag(const juce::MouseEvent &event)
    {
        if (!event.mods.isLeftButtonDown()) return;

        const auto parent{getParentComponent()};
        const auto parentBounds{parent->getBounds().toFloat()};
        const auto parentCentre{juce::Point{parentBounds.getWidth() / 2, parentBounds.getHeight() / 2}};
        auto mousePos{event.getEventRelativeTo(parent).position};
        mousePos.x -= parentBounds.getWidth() / 2;
        const auto newVal{mousePos / parentCentre};

        // Set node value.
        setValueX(juce::jlimit(
                      Params::SourcePositionRange.start,
                      Params::SourcePositionRange.end,
                      newVal.x),
                  juce::sendNotificationSync);
        setValueY(juce::jlimit(
                      Params::SourcePositionRange.start,
                      Params::SourcePositionRange.end,
                      1 - newVal.y),
                  juce::sendNotificationSync);

        setBounds();
    }

    void XYControllerComponent::Node::mouseUp(const juce::MouseEvent &event)
    {
        ignoreUnused(event);
        currentDrag.reset();
        setTooltip("(" + juce::String{value.x, 3} + ", " +
                   juce::String{value.y, 3} + ")");
    }

    void XYControllerComponent::Node::setValueX(const float newX, const juce::NotificationType notification)
    {
        value.x = newX;
        triggerChangeMessage(notification);
    }


    void XYControllerComponent::Node::setValueY(const float newY, const juce::NotificationType notification)
    {
        value.y = newY;
        triggerChangeMessage(notification);
    }


    void XYControllerComponent::Node::setBounds()
    {
        const auto bounds{getParentComponent()->getBounds().toFloat()};
        const auto x{bounds.getWidth() * (value.x + 1.f) / 2.f};
        const auto y{bounds.getHeight() - bounds.getHeight() * (value.y + 1.f) / 2.f};
        Component::setBounds(
            static_cast<int>(x - Constants::UI::NodeHalfDiameter),
            static_cast<int>(y - Constants::UI::NodeHalfDiameter),
            Constants::UI::NodeDiameter,
            Constants::UI::NodeDiameter
        );
    }

    void XYControllerComponent::Node::handleAsyncUpdate()
    {
        cancelPendingUpdate();

        const BailOutChecker checker{this};
        listeners.callChecked(checker, [&](Listener &l)
        {
            l.valueChanged(this);
        });

        if (checker.shouldBailOut()) return;

        if (const auto *handler{getAccessibilityHandler()}) {
            handler->notifyAccessibilityEvent(juce::AccessibilityEvent::valueChanged);
        }
    }

    juce::Point<float> XYControllerComponent::Node::getValue() const
    {
        return value;
    }

    void XYControllerComponent::Node::sendDragStart()
    {
        const BailOutChecker checker{this};
        listeners.callChecked(checker, [&](Listener &l) { l.dragStarted(this); });
    }

    void XYControllerComponent::Node::sendDragEnd()
    {
        const BailOutChecker checker{this};
        listeners.callChecked(checker, [&](Listener &l) { l.dragEnded(this); });
    }

    void XYControllerComponent::Node::addListener(Listener *listener)
    {
        listeners.add(listener);
    }

    void XYControllerComponent::Node::removeListener(Listener *listener)
    {
        listeners.remove(listener);
    }

    void XYControllerComponent::Node::triggerChangeMessage(const juce::NotificationType notification)
    {
        if (notification == juce::dontSendNotification) return;

        if (notification == juce::sendNotificationSync)
            handleAsyncUpdate();
        else
            triggerAsyncUpdate();
    }

    //==========================================================================

    XYControllerComponent::Node::ScopedDragNotification::ScopedDragNotification(Node &node)
        : nodeBeingDragged(node)
    {
        nodeBeingDragged.sendDragStart();
    }

    XYControllerComponent::Node::ScopedDragNotification::~ScopedDragNotification()
    {
        nodeBeingDragged.sendDragEnd();
    }

    //==========================================================================

    XYControllerComponent::ParameterAttachment::ParameterAttachment(
        juce::RangedAudioParameter &paramX,
        juce::RangedAudioParameter &paramY,
        Node &n,
        juce::UndoManager *um
    ) : node(n),
        attachmentX(paramX, [this](const float f) { setValueX(f); }, um),
        attachmentY(paramY, [this](const float f) { setValueY(f); }, um)
    {
        node.addListener(this);
        attachmentX.sendInitialUpdate();
        attachmentY.sendInitialUpdate();
        node.setTooltip("(" + juce::String{paramX.getValue() * 2.f - 1.f} + ", " + juce::String{paramY.getValue() * 2.f - 1.f} + ")");
    }

    XYControllerComponent::ParameterAttachment::~ParameterAttachment()
    {
        node.removeListener(this);
    }

    void XYControllerComponent::ParameterAttachment::setValueX(const float newX)
    {
        const juce::ScopedValueSetter svs(ignoreCallbacks, true);
        node.setValueX(newX, juce::sendNotificationSync);
        node.setBounds();
    }

    void XYControllerComponent::ParameterAttachment::setValueY(const float newY)
    {
        const juce::ScopedValueSetter svs(ignoreCallbacks, true);
        node.setValueY(newY, juce::sendNotificationSync);
        node.setBounds();
    }

    void XYControllerComponent::ParameterAttachment::valueChanged(Node *)
    {
        if (!ignoreCallbacks) {
            const auto val{node.getValue()};
            attachmentX.setValueAsPartOfGesture(val.x);
            attachmentY.setValueAsPartOfGesture(val.y);
        }
    }

    void XYControllerComponent::ParameterAttachment::dragStarted(Node *)
    {
        attachmentX.beginGesture();
        attachmentY.beginGesture();
    }

    void XYControllerComponent::ParameterAttachment::dragEnded(Node *)
    {
        attachmentX.endGesture();
        attachmentY.endGesture();
    }

    //==========================================================================

    XYControllerComponent::Attachment::Attachment(
        const juce::AudioProcessorValueTreeState &state,
        const juce::String &parameterIDX,
        const juce::String &parameterIDY,
        Node &node
    ) : attachment(std::make_unique<ParameterAttachment>(
        *state.getParameter(parameterIDX),
        *state.getParameter(parameterIDY),
        node,
        state.undoManager))
    {
    }
} // ananas::WFS
