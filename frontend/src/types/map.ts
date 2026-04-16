export interface Edge {
    toNode: number;
    weight: number;
    streetName: string;
}

export interface Node {
    intersectionId: number;
    x: number; // For visualization
    y: number; // For visualization
}

export interface CityMap {
    [key: number]: Edge[];
}
