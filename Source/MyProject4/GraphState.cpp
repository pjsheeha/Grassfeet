#include "GraphState.h"

#include "MapReaderActor.h"
#include "Util.h"

#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <stack>
#include <unordered_set>
#include <utility>
#include <vector>


AGraphState::AGraphState()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGraphState::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGraphState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


//using Point = FPoint;

struct FloodFillResult {
	uint32_t filled;
	bool has_visited;
	std::vector<uint32_t> cows;
	bool path_only;

	FloodFillResult() : filled(), has_visited(), cows(), path_only(true) {}
};

// Result is only valid when called without set_grass.
static FloodFillResult flood_fill(
	std::vector<FPoint>& points,
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
	if (points[index].cows.any()) {
		for (size_t i = 0; i < points[index].cows.size(); i++) {
			result.cows.push_back(i);
		}
	}
	result.filled++;
	fill(index, PointFillStatus::Grass);
	if (set_grass) {
		points[index].fill_status = PointFillStatus::Grass;
	}

	std::vector<bool> used(points.size());
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
					if (points[index].cows.any()) {
						for (size_t i = 0; i < points[index].cows.size(); i++) {
							result.cows.push_back(i);
						}
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
					if (points[index].cows.any()) {
						for (size_t i = 0; i < points[index].cows.size(); i++) {
							result.cows.push_back(i);
						}
					}
					fill(index, PointFillStatus::Grass);
					if (set_grass) {
						points[i].fill_status = PointFillStatus::Grass;
					}
					break;
				case PointFillStatus::Grass:
					if (points[index].cows.any()) {
						for (size_t i = 0; i < points[index].cows.size(); i++) {
							result.cows.push_back(i);
						}
					}
					result.path_only = false;
					break;
				}
			}
		}
	}

	return result;
}

static void stepOnWithoutGroupingFull(
	std::vector<FPoint>& points, uint32_t index, uint32_t max_fill,
	StepOnResult &step_on_result,
	bool set_status = true,
	std::function<void(uint32_t, PointFillStatus)> fill
	= [](uint32_t, PointFillStatus) {}
) {
	GF_LOG(L"stepOnWithoutGroupingFull, index=%d", index);

	FPoint& point = points[index];

	if (point.fill_status != PointFillStatus::Empty) {
		step_on_result.enclosures = 0;
		return;
	}

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

	std::vector<bool> visited(points.size());
	std::vector<uint32_t> cows;
	uint32_t enclosures = 0;

	for (auto& i : point.next) {
		std::function<void(uint32_t, PointFillStatus)> fill_nop =
			[](uint32_t, PointFillStatus) {};
		auto result = flood_fill(points, i, visited, false, fill_nop);

		if (result.filled > 0 && result.filled <= max_fill && !result.has_visited) {
			enclosures++;
		}
		if (result.filled > 0 && result.filled <= max_fill &&
			!result.cows.empty() && !result.has_visited) {
			// Has cow
			cows.insert(cows.end(), result.cows.begin(), result.cows.end());
		}

		if (result.filled > 0 && result.filled <= max_fill &&
			result.cows.empty() && !result.has_visited)
		{
			std::vector<bool> tmp_visited(points.size());

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
		else if (result.filled > 0 && !result.has_visited) {
			std::vector<bool> tmp_visited(points.size());
			if (status == Status::Single) {
				flood_fill(points, single_index, tmp_visited, set_status, fill);
			}
			status = Status::Multiple;
		}
	}
	if (status == Status::Single) {
		if (single_path_only) {
			std::vector<bool> tmp_visited(points.size());
			flood_fill(points, single_index, tmp_visited, set_status, fill);
		}
		else {
			GF_LOG(L"Single enclosure with non-path frontier. Not filling.");
		}
	}

	if (!set_status)
		point.fill_status = prev_fill_status;

	step_on_result.cows.insert(cows.begin(), cows.end());
	if (step_on_result.enclosures != 1 || enclosures != 1) {
		step_on_result.warning_cows.insert(cows.begin(), cows.end());
	}
	step_on_result.enclosures = enclosures;
}

static void debugStatus(std::vector<FPoint>& points)
{
	GF_LOG(L"Path:");
	for (uint32_t i = 0; i < points.size(); i++) {
		if (points[i].fill_status == PointFillStatus::Path) {
			GF_LOG(L"Path: %d", i);
		}
	}
	GF_LOG(L"Path: ==============");

	GF_LOG(L"Grass:");
	for (uint32_t i = 0; i < points.size(); i++) {
		if (points[i].fill_status == PointFillStatus::Grass) {
			GF_LOG(L"Grass: %d", i);
		}
	}
	GF_LOG(L"Grass: ==============");
}

void AGraphState::stepOn(AMapReaderActor* map_reader, FVector local_position, int32 max_fill) {
	auto& points = map_reader->GetMap();
	if (points.size() <= 0) {
		// Not initialized.
		return;
	}

	this->step_on_result.cows.clear();
	this->step_on_result.warning_cows.clear();

	stepOnWithoutGrouping(map_reader, local_position, max_fill);
	adjustGroups(map_reader);

	for (auto& c : step_on_result.cows) {
		GF_LOG(L"Cow: %d", c);
	}
	for (auto& c : step_on_result.warning_cows) {
		GF_LOG(L"Warning Cow: %d", c);
	}
}

void AGraphState::stepOnWithoutGrouping(AMapReaderActor * map_reader, FVector local_position, int32 max_fill) {
	auto& points = map_reader->GetMap();

	float min_dist = std::numeric_limits<float>::max();
	uint32_t min_i = 0;
	for (uint32_t i = 0; i < points.size(); i++) {
		FVector delta = points[i].transform.GetLocation() - local_position;
		float dist = delta.X * delta.X + delta.Y * delta.Y + delta.Z * delta.Z;
		if (dist < min_dist) {
			min_dist = dist;
			min_i = i;
		}
	}

	if (LastStep != std::numeric_limits<uint32_t>::max()) {
		if (LastStep == min_i) {
			// Same step
			points[min_i].footstep = true;
		}
		else {
			// Find the straightest path from last step to this step.
			// We assume the graph looks like a mesh.

			struct Elem {
				std::vector<uint32_t> next;
				uint32_t next_index;
			};
			std::stack<Elem> stack;
			std::vector<uint32_t> pred(points.size());
			std::vector<bool> visited(points.size());

			visited[LastStep] = true;

			do {
				uint32_t point = 0;
				if (!stack.empty()) {
					auto& cur = stack.top();
					if (cur.next_index >= cur.next.size()) {
						stack.pop();
						continue;
					}
					point = cur.next[cur.next_index];
					cur.next_index++;
				}
				else {
					point = LastStep;
				}

				std::vector<std::pair<float, uint32_t>> next;

				bool found = false;
				for (auto j : points[point].next) {
					if (!visited[j]) {
						visited[j] = true;
						pred[j] = point;
						if (j == min_i) {
							found = true;
							break;
						}

						FVector v1 = points[min_i].transform.GetLocation() - points[point].transform.GetLocation();
						FVector v2 = points[j].transform.GetLocation() - points[point].transform.GetLocation();
						v1.Normalize();
						v2.Normalize();
						float angle = FVector::DotProduct(v1, v2);

						next.push_back(std::make_pair(angle, j));
					}
				}
				if (found) break;

				std::sort(next.begin(), next.end(), std::greater<std::pair<float, uint32_t>>());

				Elem elem = Elem{ {}, 0 };
				elem.next.reserve(next.size());
				for (auto &j : next) elem.next.push_back(j.second);
				stack.push(elem);
			} while (!stack.empty());

			if (visited[min_i]) {
				auto cur = min_i;
				while (cur != LastStep) {
					points[min_i].footstep = true;
					stepOnWithoutGroupingFull(points, cur, max_fill, this->step_on_result);
					map_reader->pushPregrassQueue(cur);
					cur = pred[cur];
				}
			}
			else {
				points[min_i].footstep = true;
				stepOnWithoutGroupingFull(points, min_i, max_fill, this->step_on_result);
			}
		}
	}
	else {
		points[min_i].footstep = true;
		stepOnWithoutGroupingFull(points, min_i, max_fill, this->step_on_result);
	}
	LastStep = min_i;
	
}

void AGraphState::adjustGroups(AMapReaderActor* map_reader) {
	std::unordered_set<uint32_t> groups;
	for (auto& point : map_reader->GetMap()) {
		if (point.fill_status == PointFillStatus::Grass) {
			groups.insert(point.group);
		}
	}

	for (auto& point : map_reader->GetMap()) {
		if (groups.find(point.group) != groups.end()) {
			point.fill_status = PointFillStatus::Grass;
		}
	}
}


float AGraphState::getStatus(AMapReaderActor* map_reader){
  auto& points = map_reader->GetMap();
  int grass = 0;
  int total = points.size();
  for(auto& point : points){
  
    if (point.fill_status == PointFillStatus::Grass || point.fill_status == PointFillStatus::Path){
      grass++;
    }
  }
  return ((float)grass)/((float)total);
}
