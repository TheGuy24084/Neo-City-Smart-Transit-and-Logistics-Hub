export interface Car {
    id: number;
    currentIntersection: number;
    targetIntersection: number;
    progress: number;
    model: string;
}

export interface StateSnapshot {
    timestamp: number;
    carPositions: Record<number, { x: number, y: number }>;
    trafficLights: Record<number, boolean>;
    carCount: number; // For the history list summary
}

export interface QueueStatus {
    intersectionId: number;
    queueLength: number;
    cars: Car[];
}
