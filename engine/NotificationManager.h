#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include "GameObject.h"
#include <memory>
#include <vector>
#include <unordered_map>

namespace CMPUT350 {

/**
 * @brief Sends string notifications to the game objects that registered for them.
 *
 * Objects are held by weak pointer, so registering does not keep an object alive, and
 * destroyed objects are removed automatically the next time their notification is sent.
 */
class NotificationManager {


public:
    void Register(std::weak_ptr<GameObject> object, const std::string& key);
    void Unregister(std::weak_ptr<GameObject> object, const std::string& key);
    void Notify(const std::string& message);
private:
    std::unordered_map<std::string, std::vector<std::weak_ptr<GameObject>>> mListeners;
};

}

#endif
