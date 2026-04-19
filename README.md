# Neo-City: Smart Transit & Logistics Hub

A high-performance transit simulator building with a C++ Graph Engine and a reactive Vue 3 Meta-Visualizer.

## 🏙️ System Overview
Neo-City models a sparse urban network using sophisticated Data Structures and Algorithms (DSA) to optimize transit flow and logistics.

### 🛠️ Core Technology Stack
- **Backend ( Engine)**: C++20, STL, Mutex-based Concurrency.
- **Frontend (Visualizer)**: Vue 3, TypeScript, Tailwind CSS.
- **Routing**: Optimized Dijkstra SSSP with Priority Queue.
- **State Management**: LIFO Navigation Stack for UI.
- **Analytics**: Recursive DFS for infrastructure audit.

## 🚀 5-Day Development Sprint
- **Day 1**: Core Graph Engine (Adjacency List) and SVG Map Rendering.
- **Day 2**: Pathfinding API (Dijkstra) and shortest-path visualization.
- **Day 3**: Traffic Simulation (FIFO Queues) and History Scrubber (DLL).
- **Day 4**: Analytics Engine and Navigation Stack for UI drill-downs.
- **Day 5**: Thread-safety re-architecture, DFS District Summary, and Stress Testing.

## 🎓 Academic Defense (Viva)
For technical questions, architectural trade-offs, and DSA deep-dives, refer to the technical dossier.

## 🔨 Setup & Run
### Backend
```bash
g++ -I./include src/CityGraph.cpp src/main.cpp -o app.exe
./app.exe
```
### Frontend
```bash
npm install
npm run dev
```
