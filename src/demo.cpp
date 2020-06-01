#include <iostream>
//#include <set>
//#include <sstream>
#include <cqcppsdk/cqcppsdk.h>// 这个必须在前面


#include "src/uv/include/uv11.h"
#include<json/json.h>

using namespace cq;
using namespace std;
using Message = cq::message::Message;
using MessageSegment = cq::message::MessageSegment;
Json::Reader reader;
Json::Value root;
uv::EventLoop* loop = 0;
//sqlite3* db;
char* err_msg = 0;

DWORD WINAPI RunSocketThread(LPVOID p)
{
	loop = uv::EventLoop::DefaultLoop();

	uv::SocketAddr serverAddr("127.0.0.1", 20005, uv::SocketAddr::Ipv4);
	uv::TcpServer server(loop);
	server.setMessageCallback([](uv::TcpConnectionPtr conn, const char* data, ssize_t size)
		{
			logging::info("私聊", std::string(data, size));

			bool ret = reader.parse(std::string(data, size), root);
			if (ret)
			{
				if (root.isMember("groupid"))
				{
					if (root.isMember("message"))
					{
						try {
							send_group_message(root["groupid"].asInt64(), root["message"].asString());
							std::string str = "{\"status\" : 1, \"message\" : \"群消息成功\"}";
							conn->write(str.c_str(), str.length(), nullptr);

						}
						catch (ApiError& err)
						{
							logging::info("群聊", "群消息失败, 错误码: " + to_string(err.code));
							std::string str = "{\"status\" : 0, \"message\" : \"群消息失败\"}";
							conn->write(str.c_str(), str.length(), nullptr);
						}
					}
				}

				if (root.isMember("qqid"))
				{
					if (root.isMember("message"))
					{
						try
						{
							auto msgid = send_private_message(root["qqid"].asInt64(), root["message"].asString()); // 直接复读消息
							std::string str = "{\"status\" : 1, \"message\" : \"私聊消息成功\"}";
							conn->write(str.c_str(), str.length(), nullptr);

						}
						catch (ApiError& err)
						{
							//logging::warning("私聊", "私聊消息失败, 错误码: " + to_string(err.code));
							std::string str = "{\"status\" : 0, \"message\" : \"私聊消息失败\"}";
							conn->write(str.c_str(), str.length(), nullptr);
						}

					}
				}
			}
			else
			{
				std::string str = "{\"status\" : 0, \"message\" : \"JSON parase Failed\"}";
				conn->write(str.c_str(), str.length(), nullptr);

			}
			//root.isObject();
		});
	server.bindAndListen(serverAddr);
	loop->run();
}

CQ_INIT{
	on_enable([]{
		logging::info("启用", "插件初始化");

		HANDLE hThread;
		DWORD  threadId;
		hThread = CreateThread(NULL, 0, RunSocketThread, 0, 0, &threadId);
		logging::info("启用", "插件已启用");

	});

	on_private_message([](const PrivateMessageEvent &event) 
	{
		try 
		{
			auto msgid = send_private_message(event.user_id, event.message); // 直接复读消息
			//send_message(event.target,MessageSegment::face(111) + "这是通过 message 模块构造的消息~"); // 使用 message 模块构造消息
		}
		catch (ApiError &err) 
		{
			logging::info("私聊", "私聊消息复读失败, 错误码: " + to_string(err.code));
		}
	});

	//on_message([](const MessageEvent &event) {
	//    logging::debug("消息", "收到消息: " + event.message + "\n实际类型: " + typeid(event).name());
	//});

	on_group_message([](const GroupMessageEvent &event) 
	{
		static const set<int64_t> ENABLED_GROUPS = {512640056, 27361142};
		if (ENABLED_GROUPS.count(event.group_id) == 0) return; // 不在启用的群中, 忽略

		try {
			//auto mem_list = get_group_member_list(event.group_id); // 获取群成员列表
			//for (auto i = 0; i < min(10, static_cast<int>(mem_list.size())); i++) {
			//    msg += "昵称: " + mem_list[i].nickname + "\n"; // 拼接前十个成员的昵称
			//}
			//event.user_id
			//send_group_message(event.group_id, event.message); // 发送群消息
		}
		catch (ApiError &err) 
		{ // 忽略发送失败
			logging::warning("群聊", "消息失败, 错误码: " + to_string(err.code));
		}
		if (event.is_anonymous()) 
		{
			logging::info("群聊", "消息是匿名消息, 匿名昵称: " + event.anonymous.name);
		}
		event.block(); // 阻止当前事件传递到下一个插件
	});

	//on_group_upload([](const auto &event) { // 可以使用 auto 自动推断类型
	//    stringstream ss;
	//    ss << "您上传了一个文件, 文件名: " << event.file.name << ", 大小(字节): " << event.file.size;
	//    try {
	//        send_message(event.target, ss.str());
	//    } catch (ApiError &) {
	//    }
	//});

	on_disable([] {
		//sqlite3_free(err_msg);
		//sqlite3_close(db);
	});
}

CQ_MENU(menu_demo_1) {
	logging::info("菜单", "点击菜单1");
}

CQ_MENU(menu_demo_2) {
	send_private_message(10000, "测试");
}

//3036721833@qq.com
//08041218
