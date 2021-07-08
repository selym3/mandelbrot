#ifndef __VIEW_MODE__
#define __VIEW_MODE__

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

namespace gui
{

using Updater = std::function<void(Viewer&)>;
using EventHandler = std::function<void(Viewer&, const sf::Event&)>;

struct ViewMode
{
    const std::string name;

    Updater update;
    EventHandler handle_event;
};

ViewMode get_mouse_control()
{
    return ViewMode{
        "Mouse Control",
        [](Viewer& v) { bool b = v.is_running(); },
        [](Viewer& v, const sf::Event& e) {  bool b = v.is_running(); }
    };
}

}

#endif