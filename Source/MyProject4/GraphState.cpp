#include "GraphState.h"
#include "Util.h"

#include <functional>
#include <queue>
#include <vector>


using Point = FPoint;

struct FloodFillResult {
	uint32_t Filled;
	bool HasVisited;
	bool HasCow;
	bool PathOnly;

	FloodFillResult() : Filled(), HasVisited(), HasCow(), PathOnly(true) {}
};

// Result is only valid when called without set_grass.
static FloodFillResult FloodFill(
	UPARAM(ref) TArray<Point>& points,
	uint32_t index,
	std::vector<bool>& visited,
	bool set_grass,
	std::function<void(uint32_t, PointFillStatus)>& fill
) {
	FloodFillResult result;

	if (visited[index]) {
		result.HasVisited = true;
		return result;
	}
	if (points[index].fill_status != PointFillStatus::Empty) {
		return result;
	}
	if (points[index].has_cow) {
		result.HasCow = true;
		return result;
	}
	result.Filled++;
	fill(index, PointFillStatus::Grass);
	if (set_grass) {
		points[index].fill_status = PointFillStatus::Grass;
	}

	std::vector<bool> used(points.Num());
	used[index] = visited[index] = true;

	std::queue<uint32_t> fill_queue;
	fill_queue.push(index);

	while (!fill_queue.empty()) {
		index = fill_queue.front();
		fill_queue.pop();

		for (auto& i : points[index].next) {
			if (!used[i]) {
				switch (points[i].fill_status) {
				case PointFillStatus::Empty:
					if (visited[i]) {
						result.HasVisited = true;
						return result;
					}
					if (points[i].has_cow) {
						result.HasCow = true;
						return result;
					}
					result.Filled++;
					fill(index, PointFillStatus::Grass);
					if (set_grass) {
						points[i].fill_status = PointFillStatus::Grass;
					}
					used[i] = visited[i] = true;
					fill_queue.push(i);
					break;
				case PointFillStatus::Path:
					if (points[i].has_cow) {
						result.HasCow = true;
						return result;
					}
					fill(index, PointFillStatus::Grass);
					if (set_grass) {
						points[i].fill_status = PointFillStatus::Grass;
					}
					break;
				case PointFillStatus::Grass:
					if (points[i].has_cow) {
						result.HasCow = true;
						return result;
					}
					result.PathOnly = false;
					break;
				}
			}
		}
	}

	return result;
}

static void StepOnFull(
	UPARAM(ref) TArray<FPoint>& points, uint32_t index, uint32_t max_fill,
	bool set_status = true,
	std::function<void(uint32_t, PointFillStatus)> fill
	= [](uint32_t, PointFillStatus) {}
) {
	GF_LOG(L"StepOnFull, index=%d", index);

	Point& point = points[index];

	auto prev_fill_status = point.fill_status;

	// Mark the current point.
	fill(index, PointFillStatus::Path);
	point.fill_status = PointFillStatus::Path;

	// Flood fill.

	enum class Status {
		None, Single, Multiple
	};
	Status status = Status::None;
	uint32_t single_index = 0;
	bool single_path_only = false;

	std::vector<bool> visited(points.Num());

	for (auto& i : point.next) {
		std::function<void(uint32_t, PointFillStatus)> fill_nop =
			[](uint32_t, PointFillStatus) {};
		auto result = FloodFill(points, i, visited, false, fill_nop);

		if (result.Filled > 0 && result.Filled <= max_fill &&
			!result.HasCow && !result.HasVisited)
		{
			std::vector<bool> tmp_visited(points.Num());

			switch (status) {
			case Status::None:
				status = Status::Single;
				single_index = i;
				single_path_only = result.PathOnly;
				break;
			case Status::Single:
				status = Status::Multiple;
				FloodFill(points, single_index, tmp_visited, set_status, fill);
				FloodFill(points, i, tmp_visited, set_status, fill);
				break;
			case Status::Multiple:
				FloodFill(points, i, tmp_visited, set_status, fill);
				break;
			}
		}
	}
	if (status == Status::Single && single_path_only) {
		std::vector<bool> tmp_visited(points.Num());
		FloodFill(points, single_index, tmp_visited, set_status, fill);
	}

	if (!set_status)
		point.fill_status = prev_fill_status;
}

static void DebugStatus(UPARAM(ref) TArray<Point>& points)
{
	GF_LOG(L"Path:");
	for (uint32_t i = 0; i < (uint32_t)points.Num(); i++) {
		if (points[i].fill_status == PointFillStatus::Path) {
			GF_LOG(L"Path: %d", i);
		}
	}
	GF_LOG(L"Path: ==============");

	GF_LOG(L"Grass:");
	for (uint32_t i = 0; i < (uint32_t)points.Num(); i++) {
		if (points[i].fill_status == PointFillStatus::Grass) {
			GF_LOG(L"Grass: %d", i);
		}
	}
	GF_LOG(L"Grass: ==============");
}

void UGraphState::StepOn(UPARAM(ref) TArray<FPoint>& Points, int32 Index, int32 MaxFill) {
	if (Points.Num() <= 0) {
		// Not initialized.
		return;
	}
	StepOnFull(Points, Index, MaxFill);
	DebugStatus(Points);
}
