#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <functional>

	// 事件回调包
template<class T>
struct EventTask
{
	std::string mTag;		// 任务标签
	T mFun;		// 回调函数

	EventTask(std::string tag, T fun)
		:mTag(tag), mFun(fun) {};
};

// K:事件类型，F:回调函数类型
template<class K, class F = int>
class Event;

// 这是特殊的模板，函数类型放在参数1，参数2没用
template< class R, typename... Args, class K>
class Event<R(Args...), K>
{
private:

	using TaskFunType = std::function<R(Args...)>;

	//using KeyType = K;
	using TaskType = EventTask<TaskFunType>;
	std::vector<TaskType> mTasks;

public:

	// 添加任务
	void Add(std::string taskTag, TaskFunType taskFun);
	// 移除任务
	void Remove(std::string taskTag);

	// 执行任务
	void Invoke(Args... args);

};

// 特例化版本，将回调函数类型F拆分返回值类型R和参数包Args
// 参数1：事件索引类型；参数2：回调函数类型
template<class K, class R, typename... Args>
class Event<K, R(Args...)>
{
private:

	using TaskFunType = std::function<R(Args...)>;

	using KeyType = K;
	using TaskType = EventTask<TaskFunType>;
	using TasksType = std::vector<TaskType>;
	std::unordered_map<KeyType, TasksType> mTasks;

public:

	// 添加任务
	void Add(KeyType eventKey, std::string taskTag, TaskFunType taskFun);
	// 移除任务
	void Remove(KeyType eventKey, std::string taskTag);

	// 执行任务
	void Invoke(KeyType eventKey, Args... args);

};




template<class K, class R, typename... Args>
void Event<K, R(Args...)>::Add(KeyType eventKey, std::string taskTag, TaskFunType taskFun)
{
	mTasks[eventKey].emplace_back(taskTag, taskFun);
}

template<class K, class R, typename... Args>
void Event<K, R(Args...)>::Remove(KeyType eventKey, std::string taskTag)
{
	auto mapIt = mTasks.find(eventKey);
	if (mapIt != mTasks.end())
	{
		auto& tasks = (*mapIt).second;
		// 寻找对应tag的任务包
		auto it = tasks.begin();
		for (; it != tasks.end(); ++it)
			if ((*it).mTag == taskTag)
				break;
		// 移除任务包
		if (it != tasks.end())
		{
			tasks.erase(it);
			// 如果任务名下的任务列表为空，删除该key和其列表
			if (tasks.empty())
				mTasks.erase(mapIt);
		}
	}
}

template<class K, class R, typename... Args>
void Event<K, R(Args...)>::Invoke(KeyType eventKey, Args... args)
{
	auto mapIt = mTasks.find(eventKey);
	if (mapIt != mTasks.end())
	{
		auto& tasks = (*mapIt).second;
		for (auto& task : tasks)
		{
			task.mFun(args...);
		}
	}
}

template<class R, typename ...Args, class K>
inline void Event<R(Args...), K>::Add(std::string taskTag, TaskFunType taskFun)
{
	mTasks.emplace_back(taskTag, taskFun);
}

template<class R, typename ...Args, class K>
inline void Event<R(Args...), K>::Remove(std::string taskTag)
{
	// 寻找对应tag的任务包
	auto it = mTasks.begin();
	for (; it != mTasks.end(); ++it)
		if ((*it).mTag == taskTag)
			break;
	// 移除任务包
	if (it != mTasks.end())
		mTasks.erase(it);
}

template<class R, typename ...Args, class K>
inline void Event<R(Args...), K>::Invoke(Args ...args)
{
	for (auto& task : mTasks)
	{
		task.mFun(args...);
	}
}
