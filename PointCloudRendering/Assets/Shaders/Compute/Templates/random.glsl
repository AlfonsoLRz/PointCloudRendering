float rand(vec2 ab){
    return fract(sin(dot(ab, vec2(12.9898f, 78.233f))) * 43758.5453f);
}