// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "RelaxationTimerSelectWindow.hpp"
#include <data/RelaxationStyle.hpp>
#include <ApplicationBellRelaxation.hpp>

#include <apps-common/widgets/BellBaseLayout.hpp>
#include <common/data/BatteryStatusSwitchData.hpp>
#include <i18n/i18n.hpp>
#include <Utils.hpp>
#include <Units.hpp>
#include <LanguageUtils.hpp>

namespace
{
    using minutes = std::chrono::minutes;
    using namespace std::chrono_literals;
    constexpr minutes onceValue{minutes::zero()};
    constexpr minutes loopValue{8760h};

    std::string getOnceValueText()
    {
        return utils::translate("app_bell_relaxation_once");
    }

    std::string getLoopValueText()
    {
        return utils::translate("app_bell_relaxation_loop");
    }

    UTF8 timerValueToUTF8(minutes value)
    {
        if (value == onceValue) {
            return getOnceValueText();
        }
        if (value == loopValue) {
            return getLoopValueText();
        }
        return utils::to_string(value.count());
    }

    minutes UTF8ToTimerValue(const UTF8 &str)
    {
        if (str == getOnceValueText()) {
            return onceValue;
        }
        if (str == getLoopValueText()) {
            return loopValue;
        }
        return minutes(utils::getNumericValue<int>(str));
    }

    std::vector<UTF8> toUTF8Range(const std::vector<minutes> &spinnerRange)
    {
        std::vector<UTF8> range;
        range.reserve(spinnerRange.size());
        for (auto value : spinnerRange) {
            range.push_back(timerValueToUTF8(value));
        }
        return range;
    }
} // namespace

namespace gui
{

    RelaxationTimerSelectWindow::RelaxationTimerSelectWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::relaxation::RelaxationTimerSelectContract::Presenter> &&presenter)
        : AppWindow(app, gui::window::name::relaxationTimerSelect), presenter(std::move(presenter))
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void RelaxationTimerSelectWindow::buildInterface()
    {
        AppWindow::buildInterface();
        statusBar->setVisible(false);

        body = new BellBaseLayout(this, 0, 0, style::window_width, style::window_height);
        createTopMessage();
        createSpinner();
        createBottomDescription();
        registerCallbacks();
    }

    void RelaxationTimerSelectWindow::createTopMessage()
    {
        auto topMessage = new TextFixedSize(body->firstBox);
        topMessage->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::outer_layouts_h);
        topMessage->setFont(style::window::font::largelight);
        topMessage->setEdges(gui::RectangleEdge::None);
        topMessage->activeItem = false;
        topMessage->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        topMessage->setText(utils::translate("app_bell_relaxation_timer_title"));
        topMessage->drawUnderline(false);
    }

    void RelaxationTimerSelectWindow::createSpinner()
    {
        auto range = presenter->getTimerValuesRange();

        spinner = new StringSpinner(toUTF8Range(range), Boundaries::Fixed);
        spinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
        spinner->setFont(relaxationStyle::timerValueFont);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setEdges(RectangleEdge::None);
        spinner->setFocusEdges(RectangleEdge::None);
        spinner->set_value(timerValueToUTF8(presenter->getCurrentTimerValue()));
        spinner->onValueChanged = [this]([[maybe_unused]] const auto &value) {
            body->setMinMaxArrowsVisibility(spinner->is_min(), spinner->is_max());
            updateBottomDescription();
        };
        body->getCenterBox()->addWidget(spinner);
        body->setMinMaxArrowsVisibility(spinner->is_min(), spinner->is_max());
    }

    void RelaxationTimerSelectWindow::createBottomDescription()
    {
        bottomDescription = new gui::Text();
        bottomDescription->setMinimumSize(style::bell_base_layout::outer_layouts_w,
                                          style::bell_base_layout::outer_layouts_h);
        bottomDescription->setFont(relaxationStyle::descriptionFont);
        bottomDescription->setEdges(RectangleEdge::All);
        bottomDescription->activeItem = false;
        bottomDescription->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        bottomDescription->setVisible(true);
        body->lastBox->addWidget(bottomDescription);
        updateBottomDescription();
    }

    void RelaxationTimerSelectWindow::updateBottomDescription()
    {
        const auto minutesValue = UTF8ToTimerValue(spinner->value());

        switch (minutesValue.count()) {
        case onceValue.count():
            bottomDescription->setText(utils::translate("app_bell_relaxation_once_description"));
            break;
        case loopValue.count():
            bottomDescription->setText(utils::translate("app_bell_relaxation_loop_description"));
            break;
        default:
            bottomDescription->setText(utils::language::getCorrectMinutesNumeralForm(minutesValue.count()));
            break;
        }
    }

    void RelaxationTimerSelectWindow::registerCallbacks()
    {
        dimensionChangedCallback = [&]([[maybe_unused]] Item &item, const BoundingBox &newDim) -> bool {
            body->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };

        focusChangedCallback = [&]([[maybe_unused]] Item &item) {
            setFocusItem(focus ? body : nullptr);
            if (focus) {
                setFocusItem(body);
            }
            else {
                setFocusItem(nullptr);
            }
            return true;
        };
    }

    void RelaxationTimerSelectWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (data && typeid(*data) == typeid(RelaxationSwitchData)) {
            auto *audioSwitchData = static_cast<RelaxationSwitchData *>(data);
            audioContext          = audioSwitchData->getAudioContext();
        }
    }

    bool RelaxationTimerSelectWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (spinner->onInput(inputEvent)) {
            updateBottomDescription();
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            auto currentValue = UTF8ToTimerValue(spinner->value());
            presenter->setTimerValue(currentValue);

            auto switchToNextScreen = [this, currentValue]() {
                auto switchWindowName = (currentValue == loopValue) ? gui::window::name::relaxationRunningLoop
                                                                    : gui::window::name::relaxationRunningProgress;
                auto audioSwitchData  = std::make_unique<RelaxationSwitchData>(std::move(audioContext));
                audioSwitchData->ignoreCurrentWindowOnStack = true;
                application->switchWindow(switchWindowName, std::move(audioSwitchData));
            };

            const auto batteryState = presenter->getBatteryState();
            const units::SOC soc    = batteryState.level;
            const bool isCharging   = presenter->isBatteryCharging(batteryState.state);
            if (not presenter->isLowBatteryWindowHandled() && not isCharging &&
                presenter->isBatteryBelowLowLevelThreshold(soc)) {
                auto lowBatterySwitchData =
                    std::make_unique<AppsBatteryStatusSwitchData>(soc, isCharging, switchToNextScreen);
                lowBatterySwitchData->ignoreCurrentWindowOnStack = true;
                application->switchWindow(gui::window::name::relaxationLowBattery, std::move(lowBatterySwitchData));
                presenter->handleLowBatteryWindow();
            }
            else {
                switchToNextScreen();
            }

            return true;
        }
        return AppWindow::onInput(inputEvent);
    }
} // namespace gui
