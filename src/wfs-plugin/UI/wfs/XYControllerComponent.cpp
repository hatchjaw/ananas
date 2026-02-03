#include "XYControllerComponent.h"
#include "../../Utils.h"

namespace ananas::WFS
{
    XYControllerComponent::XYControllerComponent(const uint numNodesToCreate, juce::AudioProcessorValueTreeState &apvts)
        : state(apvts)
    {
        apvts.addParameterListener(Params::SpeakerSpacing.id, this);
        calculateGridSpacingX();

        for (uint n{0}; n < numNodesToCreate; ++n) {
            // Add a node for each sound source
            const auto node{nodes.add(new Node{n})};
            addAndMakeVisible(node);
            node->setBroughtToFrontOnMouseClick(true);

            // Add a parameter attachment
            attachments.add(new Attachment{n, apvts, *node});
        }
    }

    void XYControllerComponent::paint(juce::Graphics &g)
    {
        g.fillAll(juce::Colour{0.f, 0.f, 0.f, .1f});

        g.setColour(Constants::UI::XYControllerGridlineColour);
        const auto right{getBounds().toFloat().getWidth()};

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

        const auto yZero{Constants::MaxYMetres * getHeight() / (Constants::MaxYMetres - Constants::MinYMetres)};
        const auto unit{getHeight() / (Constants::MaxYMetres - Constants::MinYMetres)};

        // y gridlines
        for (int y{Constants::MinYMetres}; y < Constants::MaxYMetres; ++y) {
            g.drawHorizontalLine(yZero - y * unit, 0.f, right);
        }
    }

    void XYControllerComponent::resized()
    {
        for (const auto &node: nodes) {
            node->setBounds();
        }

        calculateGridSpacingX();
    }

    void XYControllerComponent::calculateGridSpacingX()
    {
        // x grid spacing is half the width of the XY controller divided by
        // speaker spacing times the number of modules (i.e. half the number of
        // speakers).
        const auto halfArrayWidth{getSpeakerArrayWidth() / 2};
        const auto halfXYWidth{getWidth() / 2};
        xGridSpacing = halfXYWidth / halfArrayWidth;
    }

    float XYControllerComponent::getSpeakerArrayWidth() const
    {
        return state.getRawParameterValue(Params::SpeakerSpacing.id)->load() * 2.f * Constants::NumModules;
    }

    void XYControllerComponent::parameterChanged(const juce::String &parameterID, const float newValue)
    {
        juce::ignoreUnused(newValue);

        if (parameterID == Params::SpeakerSpacing.id) {
            calculateGridSpacingX();
            for (const auto &node: nodes) {
                node->updateTooltip();
            }
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
        const auto origin{
            juce::Point{
                parentBounds.getWidth() / 2,
                Constants::MaxYMetres * parentBounds.getHeight() /
                (Constants::MaxYMetres - Constants::MinYMetres)
            }
        };
        auto mousePos{event.getEventRelativeTo(parent).position};

        mousePos -= origin;

        auto newVal{mousePos};

        // Normalise x to [-1, 1]
        newVal.x /= (parentBounds.getWidth() / 2.f);

        // Normalize y to [-1, 1]
        if (mousePos.y <= 0) {
            // Above zero: map [-yZero, 0] to [1, 0]
            newVal.y = -mousePos.y / origin.y;
        } else {
            // Below zero: map [0, height-yZero] to [0, -1]
            newVal.y = -mousePos.y / (parentBounds.getHeight() - origin.y);
        }

        // Set node value.
        setValueX(juce::jlimit(
                      Params::SourcePositionRange.start,
                      Params::SourcePositionRange.end,
                      newVal.x),
                  juce::sendNotificationSync);
        setValueY(juce::jlimit(
                      Params::SourcePositionRange.start,
                      Params::SourcePositionRange.end,
                      newVal.y),
                  juce::sendNotificationSync);

        setBounds();
    }

    void XYControllerComponent::Node::mouseUp(const juce::MouseEvent &event)
    {
        ignoreUnused(event);
        currentDrag.reset();
        updateTooltip();
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
        const auto parentBounds{getParentComponent()->getBounds().toFloat()};
        const auto origin{
            juce::Point{
                parentBounds.getWidth() / 2,
                Constants::MaxYMetres * parentBounds.getHeight() /
                (Constants::MaxYMetres - Constants::MinYMetres)
            }
        };

        const auto x{(value.x + 1.f) * origin.x};
        auto y{0.f};

        if (value.y >= 0) {
            y = origin.y - origin.y * value.y;
        } else {
            y = origin.y - (parentBounds.getHeight() - origin.y) * value.y;
        }

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

    void XYControllerComponent::Node::updateTooltip()
    {
        const XYControllerComponent *xy{dynamic_cast<XYControllerComponent *>(getParentComponent())};
        const auto w{xy->getSpeakerArrayWidth()};
        setTooltip(Utils::normalisedPointToCoordinateMetres(
            value,
            juce::Point{
                -w / 2,
                static_cast<float>(Constants::MinYMetres)
            },
            juce::Point{
                w / 2,
                static_cast<float>(Constants::MaxYMetres)
            }));
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
        const uint sourceIndex,
        const juce::AudioProcessorValueTreeState &state,
        Node &n,
        juce::UndoManager *um
    ) : node(n),
        attachmentX(*state.getParameter(Params::getSourcePositionParamID(sourceIndex, SourcePositionAxis::X)), [this](const float f) { setValueX(f); }, um),
        attachmentY(*state.getParameter(Params::getSourcePositionParamID(sourceIndex, SourcePositionAxis::Y)), [this](const float f) { setValueY(f); }, um)
    {
        node.addListener(this);
        attachmentX.sendInitialUpdate();
        attachmentY.sendInitialUpdate();
        const auto w{state.getRawParameterValue(Params::SpeakerSpacing.id)->load() * 2.f * Constants::NumModules};
        node.setTooltip(Utils::normalisedPointToCoordinateMetres(
            juce::Point{
                state.getRawParameterValue(Params::getSourcePositionParamID(sourceIndex, SourcePositionAxis::X))->load(),
                state.getRawParameterValue(Params::getSourcePositionParamID(sourceIndex, SourcePositionAxis::Y))->load(),
            },
            juce::Point{-w / 2, static_cast<float>(Constants::MinYMetres)},
            juce::Point{w / 2, static_cast<float>(Constants::MaxYMetres)}
        ));
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
        const uint sourceIndex,
        juce::AudioProcessorValueTreeState &state,
        Node &node
    ) : attachment(std::make_unique<ParameterAttachment>(sourceIndex, state, node, state.undoManager))
    {
    }
} // ananas::WFS
