#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <functional>

	// �¼��ص���
template<class T>
struct EventTask
{
	std::string mTag;		// �����ǩ
	T mFun;		// �ص�����

	EventTask(std::string tag, T fun)
		:mTag(tag), mFun(fun) {};
};

// K:�¼����ͣ�F:�ص���������
template<class K, class F = int>
class Event;

// ���������ģ�壬�������ͷ��ڲ���1������2û��
template< class R, typename... Args, class K>
class Event<R(Args...), K>
{
private:

	using TaskFunType = std::function<R(Args...)>;

	//using KeyType = K;
	using TaskType = EventTask<TaskFunType>;
	std::vector<TaskType> mTasks;

public:

	// �������
	void Add(std::string taskTag, TaskFunType taskFun);
	// �Ƴ�����
	void Remove(std::string taskTag);

	// ִ������
	void Invoke(Args... args);

};

// �������汾�����ص���������F��ַ���ֵ����R�Ͳ�����Args
// ����1���¼��������ͣ�����2���ص���������
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

	// �������
	void Add(KeyType eventKey, std::string taskTag, TaskFunType taskFun);
	// �Ƴ�����
	void Remove(KeyType eventKey, std::string taskTag);

	// ִ������
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
		// Ѱ�Ҷ�Ӧtag�������
		auto it = tasks.begin();
		for (; it != tasks.end(); ++it)
			if ((*it).mTag == taskTag)
				break;
		// �Ƴ������
		if (it != tasks.end())
		{
			tasks.erase(it);
			// ����������µ������б�Ϊ�գ�ɾ����key�����б�
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
	// Ѱ�Ҷ�Ӧtag�������
	auto it = mTasks.begin();
	for (; it != mTasks.end(); ++it)
		if ((*it).mTag == taskTag)
			break;
	// �Ƴ������
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
