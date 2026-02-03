#ifndef XYCONTROLLERCOMPONENT_H
#define XYCONTROLLERCOMPONENT_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace ananas::WFS
{
    class XYControllerComponent final : public juce::Component,
                                        public juce::AudioProcessorValueTreeState::Listener
    {
    public:
        XYControllerComponent(uint numNodesToCreate, juce::AudioProcessorValueTreeState &apvts);

        void paint(juce::Graphics &g) override;

        void resized() override;

        void calculateGridSpacingX();

        float getSpeakerArrayWidth() const;

        void parameterChanged(const juce::String &parameterID, float newValue) override;

    private:
        class Node final : public Component,
                           public juce::AsyncUpdater,
                           public juce::SettableTooltipClient
        {
        public:
            class Listener;

            explicit Node(uint idx);

            void paint(juce::Graphics &g) override;

            void mouseDown(const juce::MouseEvent &event) override;

            void mouseDrag(const juce::MouseEvent &event) override;

            void mouseUp(const juce::MouseEvent &event) override;

            void setValueX(float newX, juce::NotificationType notification);

            void setValueY(float newY, juce::NotificationType notification);

            void setBounds();

            void handleAsyncUpdate() override;

            [[nodiscard]] juce::Point<float> getValue() const;

            void sendDragStart();

            void sendDragEnd();

            void updateTooltip();

            void addListener(Listener *listener);

            void removeListener(Listener *listener);

            class Listener
            {
            public:
                virtual ~Listener() = default;

                virtual void valueChanged(Node *) = 0;

                virtual void dragStarted(Node *) = 0;

                virtual void dragEnded(Node *) = 0;
            };

            class ScopedDragNotification
            {
            public:
                explicit ScopedDragNotification(Node &node);

                ~ScopedDragNotification();

            private:
                Node &nodeBeingDragged;

                JUCE_DECLARE_NON_MOVEABLE(ScopedDragNotification)

                JUCE_DECLARE_NON_COPYABLE(ScopedDragNotification)
            };

        private:
            void triggerChangeMessage(juce::NotificationType notification);

            uint index{};
            juce::Point<float> value{};
            juce::ListenerList<Listener> listeners;
            std::unique_ptr<ScopedDragNotification> currentDrag;
        };

        class ParameterAttachment final : Node::Listener
        {
        public:
            ParameterAttachment(uint sourceIndex,
                                const juce::AudioProcessorValueTreeState &state,
                                Node &n,
                                juce::UndoManager *um = nullptr);

            ~ParameterAttachment() override;

        private:
            void setValueX(float newX);

            void setValueY(float newY);

            void valueChanged(Node *node) override;

            void dragStarted(Node *node) override;

            void dragEnded(Node *node) override;

            Node &node;
            juce::ParameterAttachment attachmentX, attachmentY;
            bool ignoreCallbacks{false};
        };

        class Attachment
        {
        public:
            Attachment(uint sourceIndex,
                       juce::AudioProcessorValueTreeState &state,
                       Node &node);

        private:
            std::unique_ptr<ParameterAttachment> attachment;
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Attachment);
        };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XYControllerComponent)

        juce::OwnedArray<Node> nodes;
        juce::OwnedArray<Attachment> attachments;
        int xGridSpacing{};
        juce::AudioProcessorValueTreeState &state;
    };
} // ananas::WFS

#endif //XYCONTROLLERCOMPONENT_H
