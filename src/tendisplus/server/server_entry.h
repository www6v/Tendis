#ifndef SRC_TENDISPLUS_SERVER_SERVER_ENTRY_H_
#define SRC_TENDISPLUS_SERVER_SERVER_ENTRY_H_

#include <vector>
#include <utility>
#include <memory>
#include <map>

#include "tendisplus/network/network.h"
#include "tendisplus/network/worker_pool.h"
#include "tendisplus/server/server_params.h"

namespace tendisplus {
class NetSession;
class NetworkAsio;

class ServerEntry: public std::enable_shared_from_this<ServerEntry> {
 public:
    ServerEntry();
    ServerEntry(const ServerEntry&) = delete;
    ServerEntry(ServerEntry&&) = delete;
    Status startup(const std::shared_ptr<ServerParams>& cfg);
    template <typename fn>
    void schedule(fn&& task) {
        _executor->schedule(std::forward<fn>(task));
    }
    void addSession(std::unique_ptr<NetSession> sess);
    void endSession(uint64_t connId);
    void processReq(uint64_t connId);
    void stop();
    void waitStopComplete();

 private:
    // NOTE(deyukong): _isRunning = true -> running
    // _isRunning = false && _isStopped = false -> stopping in progress
    // _isRunning = false && _isStopped = true -> stop complete
    std::atomic<bool> _isRunning;
    std::atomic<bool> _isStopped;
    std::mutex _mutex;
    std::condition_variable _eventCV;
    std::unique_ptr<NetworkAsio> _network;
    std::map<uint64_t, std::unique_ptr<NetSession>> _sessions;
    std::unique_ptr<WorkerPool> _executor;
};
}  // namespace tendisplus

#endif  // SRC_TENDISPLUS_SERVER_SERVER_ENTRY_H_
