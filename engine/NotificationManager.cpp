#include "NotificationManager.h"
#include "GameObject.h"
#include <algorithm>

namespace CMPUT350 {

bool isObjectExpired(const std::weak_ptr<GameObject> obj);
bool isObjectListening(const std::vector<std::weak_ptr<GameObject>>& vec, const std::weak_ptr<GameObject>& target);

/**
 * @brief Registers an object to receive a notification.
 *
 * @param object The object that will receive the notification.
 * @param key The notification to listen for.
 *
 * Registering the same object for the same key more than once has no extra effect.
 */
void NotificationManager::Register(std::weak_ptr<GameObject> object, const std::string& key) {
    if (mListeners.find(key) != mListeners.end()) {
        if (!isObjectListening(mListeners[key], object)) {
            mListeners[key].push_back(object);
        }
        return;
    }

    mListeners[key] = {object};
}
/**
 * @brief Stops an object from receiving a notification.
 *
 * @param object The object to remove.
 * @param key The notification it should no longer receive.
 *
 * Does nothing if the object was not registered for the key.
 */
void NotificationManager::Unregister(std::weak_ptr<GameObject> object, const std::string& key) {
    if (mListeners.find(key) != mListeners.end()) {
        int idx = -1;
        for (int i = 0; i < mListeners[key].size(); ++i) {
            auto ptr =  mListeners[key][i];
            // https://en.cppreference.com/cpp/memory/weak_ptr/owner_before
            if (!ptr.owner_before(object) && !object.owner_before(ptr)) {
                idx = i;
                break;
            }
        }

        if (idx != -1) {
            mListeners[key].erase(mListeners[key].begin() + idx);
        }
    }
}

/**
 * @brief Sends a notification to every object registered for it.
 *
 * @param message The notification. ReceiveNotification(message) is called on every live
 *                object registered for it, in the order they registered.
 *
 * Expired objects are skipped and then removed from the listener list.
 */
void NotificationManager::Notify(const std::string& message) {
    if (mListeners.find(message) != mListeners.end()) {
        for (const auto& ptr: mListeners[message]) {
            if (!ptr.expired()) {
                auto ptrShared = ptr.lock();
                ptrShared->ReceiveNotification(message);
            }
        }

        mListeners[message].erase(std::remove_if(mListeners[message].begin(), mListeners[message].end(), isObjectExpired), mListeners[message].end());
    }
}

/**
 * @brief Predicate used to remove destroyed objects from a listener list.
 * @return true if the object no longer exists.
 */
bool isObjectExpired(const std::weak_ptr<GameObject> obj) { return obj.expired(); }

/**
 * @brief Checks whether an object is already in a listener list.
 *
 * @param vec The listener list.
 * @param target The object to look for.
 * @return true if target refers to the same object as an entry in vec.
 */
bool isObjectListening(const std::vector<std::weak_ptr<GameObject>>& vec, const std::weak_ptr<GameObject>& target) {
    for (const auto& ptr: vec) {
        // https://en.cppreference.com/cpp/memory/weak_ptr/owner_before
        if (!ptr.owner_before(target) && !target.owner_before(ptr)) {
            return true;
        }
    }

    return false;
}

}