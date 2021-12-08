#ifndef SIGNATURE_CALCULATOR_H
#define SIGNATURE_CALCULATOR_H

#include <memory>
#include <string>
#include <future>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <optional>
#include <cassert>
#include <condition_variable>

class IHashSaver;
class IDataProvider;

namespace Hash { class IHashCalculator; }

namespace Calculator
{

class CalculatorManager
{
public:
	CalculatorManager(const std::shared_ptr<IDataProvider> & dataProvider,
					  const std::shared_ptr<IHashSaver> & hashSaver,
					  const std::shared_ptr<Hash::IHashCalculator> & hashCalculator,
					  const size_t readSize);

	~CalculatorManager();
	void Start();

private:
	void ThreadWorker(int threadIndex);

	const std::shared_ptr<IDataProvider> m_dataProvider;
	const std::shared_ptr<IHashSaver> m_hashSaver;
	const std::shared_ptr<Hash::IHashCalculator> m_hashCalculator;
	const size_t m_bytesToRead;
	unsigned int m_numberOfAvailableThreads;

	std::atomic_bool m_stopExecution {false};

	std::vector<std::thread> m_threadsPool;
	std::vector<std::mutex> m_threadsMutexes;
	std::vector<std::condition_variable> m_threadsConditionalVariables;
	std::vector<std::optional<std::packaged_task<std::string()>>> m_threadsTasksPool;
};
} // namespace Calculator

#endif
