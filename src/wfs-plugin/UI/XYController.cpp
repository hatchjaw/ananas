#include "XYController.h"

#include "../Utils.h"

namespace ananas::WFS
{
    XYController::XYController(const uint numNodesToCreate, const juce::AudioProcessorValueTreeState &apvts)
    {
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

    void XYController::paint(juce::Graphics &g)
    {
        g.fillAll(juce::Colour{0.f, 0.f, 0.f, .1f});
    }

    void XYController::resized()
    {
        for (const auto &node: nodes) {
            node->setBounds();
        }
    }

    //==========================================================================

    XYController::Node::Node(const uint idx) : index(idx)
    {
    }

    void XYController::Node::paint(juce::Graphics &g)
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

    void XYController::Node::mouseDown(const juce::MouseEvent &event)
    {
        ignoreUnused(event);
        currentDrag.reset();
        currentDrag = std::make_unique<ScopedDragNotification>(*this);
    }

    void XYController::Node::mouseDrag(const juce::MouseEvent &event)
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

    void XYController::Node::mouseUp(const juce::MouseEvent &event)
    {
        ignoreUnused(event);
        currentDrag.reset();
    }

    void XYController::Node::setValueX(const float newX, const juce::NotificationType notification)
    {
        value.x = newX;
        triggerChangeMessage(notification);
    }


    void XYController::Node::setValueY(const float newY, const juce::NotificationType notification)
    {
        value.y = newY;
        triggerChangeMessage(notification);
    }


    void XYController::Node::setBounds()
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

    void XYController::Node::handleAsyncUpdate()
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

    juce::Point<float> XYController::Node::getValue() const
    {
        return value;
    }

    void XYController::Node::sendDragStart()
    {
        const BailOutChecker checker{this};
        listeners.callChecked(checker, [&](Listener &l) { l.dragStarted(this); });
    }

    void XYController::Node::sendDragEnd()
    {
        const BailOutChecker checker{this};
        listeners.callChecked(checker, [&](Listener &l) { l.dragEnded(this); });
    }

    void XYController::Node::addListener(Listener *listener)
    {
        listeners.add(listener);
    }

    void XYController::Node::removeListener(Listener *listener)
    {
        listeners.remove(listener);
    }

    void XYController::Node::triggerChangeMessage(const juce::NotificationType notification)
    {
        if (notification == juce::dontSendNotification) return;

        if (notification == juce::sendNotificationSync)
            handleAsyncUpdate();
        else
            triggerAsyncUpdate();
    }

    //==========================================================================

    XYController::Node::ScopedDragNotification::ScopedDragNotification(Node &node)
        : nodeBeingDragged(node)
    {
        nodeBeingDragged.sendDragStart();
    }

    XYController::Node::ScopedDragNotification::~ScopedDragNotification()
    {
        nodeBeingDragged.sendDragEnd();
    }

    //==========================================================================

    XYController::ParameterAttachment::ParameterAttachment(
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
    }

    XYController::ParameterAttachment::~ParameterAttachment()
    {
        node.removeListener(this);
    }

    void XYController::ParameterAttachment::setValueX(const float newX)
    {
        const juce::ScopedValueSetter svs(ignoreCallbacks, true);
        node.setValueX(newX, juce::sendNotificationSync);
        node.setBounds();
    }

    void XYController::ParameterAttachment::setValueY(const float newY)
    {
        const juce::ScopedValueSetter svs(ignoreCallbacks, true);
        node.setValueY(newY, juce::sendNotificationSync);
        node.setBounds();
    }

    void XYController::ParameterAttachment::valueChanged(Node *)
    {
        if (!ignoreCallbacks) {
            const auto val{node.getValue()};
            attachmentX.setValueAsPartOfGesture(val.x);
            attachmentY.setValueAsPartOfGesture(val.y);
        }
    }

    void XYController::ParameterAttachment::dragStarted(Node *)
    {
        attachmentX.beginGesture();
        attachmentY.beginGesture();
    }

    void XYController::ParameterAttachment::dragEnded(Node *)
    {
        attachmentX.endGesture();
        attachmentY.endGesture();
    }

    //==========================================================================

    XYController::Attachment::Attachment(
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
