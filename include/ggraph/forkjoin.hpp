#ifndef GGRAPH_FORKJOIN_HPP
#define GGRAPH_FORKJOIN_HPP

#include <ggraph/fork.hpp>
#include <ggraph/join.hpp>
#include <ggraph/operation.hpp>

namespace ggraph {

class forkjoin final : public operation {
public:
	virtual
	~forkjoin();

	forkjoin(const ggraph::fork & fork, const ggraph::join & join);

	virtual std::string
	debug_string() const override;

	virtual std::unique_ptr<operation>
	copy() const override;

	inline const ggraph::fork &
	fork() const noexcept
	{
		return fork_;
	}

	inline const ggraph::join &
	join() const noexcept
	{
		return join_;
	}

private:
	ggraph::fork fork_;
	ggraph::join join_;
};

static inline bool
is_forkjoin(const ggraph::operation & operation) noexcept
{
	return dynamic_cast<const forkjoin*>(&operation) != nullptr;
}

static inline std::unique_ptr<operation>
create_forkjoin(const ggraph::fork & fork, const ggraph::join & join)
{
	return std::unique_ptr<operation>(new forkjoin(fork, join));
}

}

#endif
