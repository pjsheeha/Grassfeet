#include "GraphState.h"
#include "Util.h"

#include <functional>
#include <queue>
#include <vector>


using Point = FPoint;

struct FloodFillResult {
	uint32_t filled;
	bool has_visited;
	bool has_cow;
	bool path_only;

	FloodFillResult() : filled(), has_visited(), has_cow(), path_only(true) {}
};

// Result is only valid when called without set_grass.
static FloodFillResult flood_fill(
	UPARAM(ref) TArray<Point>& points,
	uint32_t index,
	std::vector<bool>& visited,
	bool set_grass,
	std::function<void(uint32_t, PointFillStatus)>& fill
) {
	FloodFillResult result;

	if (visited[index]) {
		result.has_visited = true;
		return result;
	}
	if (points[index].fill_status != PointFillStatus::Empty) {
		return result;
	}
	if (points[index].has_cow) {
		result.has_cow = true;
		return result;
	}
	result.filled++;
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
						result.has_visited = true;
						return result;
					}
					if (points[i].has_cow) {
						result.has_cow = true;
						return result;
					}
					result.filled++;
					fill(index, PointFillStatus::Grass);
					if (set_grass) {
						points[i].fill_status = PointFillStatus::Grass;
					}
					used[i] = visited[i] = true;
					fill_queue.push(i);
					break;
				case PointFillStatus::Path:
					if (points[i].has_cow) {
						result.has_cow = true;
						return result;
					}
					fill(index, PointFillStatus::Grass);
					if (set_grass) {
						points[i].fill_status = PointFillStatus::Grass;
					}
					break;
				case PointFillStatus::Grass:
					if (points[i].has_cow) {
						result.has_cow = true;
						return result;
					}
					result.path_only = false;
					break;
				}
			}
		}
	}

	return result;
}

static void stepOnFull(
	UPARAM(ref) TArray<FPoint>& points, uint32_t index, uint32_t max_fill,
	bool set_status = true,
	std::function<void(uint32_t, PointFillStatus)> fill
	= [](uint32_t, PointFillStatus) {}
) {
	GF_LOG(L"stepOnFull, index=%d", index);

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
		auto result = flood_fill(points, i, visited, false, fill_nop);

		if (result.filled > 0 && result.filled <= max_fill &&
			!result.has_cow && !result.has_visited)
		{
			std::vector<bool> tmp_visited(points.Num());

			switch (status) {
			case Status::None:
				status = Status::Single;
				single_index = i;
				single_path_only = result.path_only;
				break;
			case Status::Single:
				status = Status::Multiple;
				flood_fill(points, single_index, tmp_visited, set_status, fill);
				flood_fill(points, i, tmp_visited, set_status, fill);
				break;
			case Status::Multiple:
				flood_fill(points, i, tmp_visited, set_status, fill);
				break;
			}
		}
	}
	if (status == Status::Single && single_path_only) {
		std::vector<bool> tmp_visited(points.Num());
		flood_fill(points, single_index, tmp_visited, set_status, fill);
	}

	if (!set_status)
		point.fill_status = prev_fill_status;
}

static void debugStatus(UPARAM(ref) TArray<Point>& points)
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

void UGraphState::stepOn(UPARAM(ref) TArray<FPoint>& points, int32 index, int32 max_fill) {
	if (points.Num() <= 0) {
		// Not initialized.
		return;
	}
	stepOnFull(points, index, max_fill);
	debugStatus(points);
}
