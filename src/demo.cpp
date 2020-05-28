#include <iostream>
#include <set>
#include <sstream>

#include <cqcppsdk/cqcppsdk.h>

#include<json/json.h>
#include <windows.h>

using namespace cq;
using namespace std;
using Message = cq::message::Message;
using MessageSegment = cq::message::MessageSegment;
Json::Reader reader;
Json::Value root;
CQ_INIT {
    on_enable([] { 
        logging::info("启用", "插件已启用"); 
        //HANDLE hPipe = CreateNamedPipe(TEXT("\\\\.\\Pipe\\mypipe"), //管道名
        //                               PIPE_ACCESS_DUPLEX, //管道类型
        //                               PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, //管道参数
        //                               PIPE_UNLIMITED_INSTANCES, //管道能创建的最大实例数量
        //                               0, //输出缓冲区长度 0表示默认
        //                               0, //输入缓冲区长度 0表示默认
        //                               NMPWAIT_WAIT_FOREVER, //超时时间
        //                               NULL);

        });

    on_private_message([](const PrivateMessageEvent &event) {
        try {
            auto msgid = send_private_message(event.user_id, event.message); // 直接复读消息
            logging::info_success("私聊", "私聊消息复读完成, 消息 Id: " + to_string(msgid));
            send_message(event.target,
                         MessageSegment::face(111) + "这是通过 message 模块构造的消息~"); // 使用 message 模块构造消息
        } catch (ApiError &err) {
            logging::warning("私聊", "私聊消息复读失败, 错误码: " + to_string(err.code));
        }
    });

    on_message([](const MessageEvent &event) {
        logging::debug("消息", "收到消息: " + event.message + "\n实际类型: " + typeid(event).name());
    });

    on_group_message([](const GroupMessageEvent &event) {
        static const set<int64_t> ENABLED_GROUPS = {512640056, 27361142};
        if (ENABLED_GROUPS.count(event.group_id) == 0) return; // 不在启用的群中, 忽略

        try {
            //send_message(event.target, event.message); // 复读
            auto mem_list = get_group_member_list(event.group_id); // 获取群成员列表
            string msg="不要AT我啊。";
            //for (auto i = 0; i < min(10, static_cast<int>(mem_list.size())); i++) {
            //    msg += "昵称: " + mem_list[i].nickname + "\n"; // 拼接前十个成员的昵称
            //}
            send_group_message(event.group_id, event.message); // 发送群消息
        } catch (ApiError &err) { // 忽略发送失败
            logging::warning("群聊", "消息失败, 错误码: " + to_string(err.code));
        }
        if (event.is_anonymous()) {
            logging::info("群聊", "消息是匿名消息, 匿名昵称: " + event.anonymous.name);
        }
        event.block(); // 阻止当前事件传递到下一个插件
    });

    on_group_upload([](const auto &event) { // 可以使用 auto 自动推断类型
        stringstream ss;
        ss << "您上传了一个文件, 文件名: " << event.file.name << ", 大小(字节): " << event.file.size;
        try {
            send_message(event.target, ss.str());
        } catch (ApiError &) {
        }
    });
}

CQ_MENU(menu_demo_1) {
    logging::info("菜单", "点击菜单1");
}

CQ_MENU(menu_demo_2) {
    send_private_message(10000, "测试");
}
