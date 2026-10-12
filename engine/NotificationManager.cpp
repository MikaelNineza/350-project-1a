#include "NotificationManager.h"
#include "GameObject.h"
#include <algorithm>

namespace CMPUT350 {

bool isObjectExpired(const std::weak_ptr<GameObject> obj);
bool isObjectListening(const std::vector<std::weak_ptr<GameObject>>& vec, const std::weak_ptr<GameObject>& target);

void NotificationManager::Register(std::weak_ptr<GameObject> object, const std::string& key) {
    if (mListeners.find(key) != mListeners.end()) {
        if (!isObjectListening(mListeners[key], object)) {
            mListeners[key].push_back(object);
        }
        return;
    }

    mListeners[key] = {object};
}
void NotificationManager::Unregister(std::weak_ptr<GameObject> object, const std::string& key) {
    if (mListeners.find(key) != mListeners.end()) {
        int idx = -1;
        for (int i = 0; i < mListeners[key].size(); ++i) {
            auto ptr =  mListeners[key][i];
            // https://en.cppreference.com/cpp/memory/weak_ptr/owner_before
            if (!ptr.owner_before(object) && object.owner_before(ptr)) {
                idx = i;
                break;
            }
        }

        if (idx != -1) {
            mListeners[key].erase(mListeners[key].begin() + idx);
        }
    }
}

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

bool isObjectExpired(const std::weak_ptr<GameObject> obj) { return obj.expired(); }

bool isObjectListening(const std::vector<std::weak_ptr<GameObject>>& vec, const std::weak_ptr<GameObject>& target) {
    for (const auto& ptr: vec) {
        // https://en.cppreference.com/cpp/memory/weak_ptr/owner_before
        if (!ptr.owner_before(target) && target.owner_before(ptr)) {
            return true;
        }
    }

    return false;
}

}