#include "Skybox.h"

static const char* SKY_VERT = R"(
    #version 330 core
    layout(location = 0) in vec3 aPos;

    uniform mat4 view;
    uniform mat4 projection;

    out vec3 rayDir;

    void main() {
        // Pass the vertex position as a direction for the fragment shader.
        // The skybox cube is centered on the camera, so vertex positions
        // directly correspond to view directions.
        rayDir = aPos;

        // Remove translation from view matrix — the sky moves with the camera.
        // Only rotation matters so the sky rotates as you look around
        // but never gets closer or farther.
        mat4 rotView = mat4(mat3(view));
        vec4 pos = projection * rotView * vec4(aPos, 1.0);

        // Set z = w so the skybox always fails the depth test against
        // real geometry but passes against the cleared depth buffer.
        // This means it draws everywhere that nothing else drew.
        gl_Position = pos.xyww;
    }
)";

static const char* SKY_FRAG = R"(
    #version 330 core
    in vec3 rayDir;
    out vec4 FragColor;

    uniform float time;

    void main() {
        vec3 dir = normalize(rayDir);

        // How high up are we looking? -1 = straight down, +1 = straight up
        float y = dir.y;

        // Sky color: deep blue at zenith, lighter blue-white at horizon
        vec3 zenith = vec3(0.15, 0.3, 0.65);    // Deep blue
        vec3 horizon = vec3(0.6, 0.7, 0.85);     // Pale blue-white
        vec3 ground = vec3(0.25, 0.22, 0.2);     // Brownish below horizon

        vec3 sky;
        if (y > 0.0) {
            // Above horizon: blend from horizon to zenith
            float t = pow(y, 0.6);
            sky = mix(horizon, zenith, t);
        } else {
            // Below horizon: fade to ground color
            float t = pow(-y, 0.4);
            sky = mix(horizon, ground, t);
        }

        // Sun — a bright spot in the sky
        vec3 sunDir = normalize(vec3(0.4, 0.35, -0.8));
        float sunDot = max(dot(dir, sunDir), 0.0);

        // Sharp sun disk
        float sun = pow(sunDot, 800.0);
        sky += vec3(1.0, 0.95, 0.8) * sun * 3.0;

        // Sun glow (softer halo around the sun)
        float glow = pow(sunDot, 8.0);
        sky += vec3(1.0, 0.8, 0.4) * glow * 0.2;

        // Horizon glow near the sun
        float horizonGlow = pow(1.0 - abs(y), 8.0) * pow(sunDot, 2.0);
        sky += vec3(1.0, 0.7, 0.3) * horizonGlow * 0.3;

        FragColor = vec4(sky, 1.0);
    }
)";

// Unit cube vertices — just positions, no normals needed
static float skyboxVertices[] = {
    -1, 1,-1,  -1,-1,-1,   1,-1,-1,   1,-1,-1,   1, 1,-1,  -1, 1,-1,
    -1,-1, 1,  -1,-1,-1,  -1, 1,-1,  -1, 1,-1,  -1, 1, 1,  -1,-1, 1,
     1,-1,-1,   1,-1, 1,   1, 1, 1,   1, 1, 1,   1, 1,-1,   1,-1,-1,
    -1,-1, 1,  -1, 1, 1,   1, 1, 1,   1, 1, 1,   1,-1, 1,  -1,-1, 1,
    -1, 1,-1,   1, 1,-1,   1, 1, 1,   1, 1, 1,  -1, 1, 1,  -1, 1,-1,
    -1,-1,-1,  -1,-1, 1,   1,-1, 1,   1,-1, 1,   1,-1,-1,  -1,-1,-1,
};

bool Skybox::init() {
    if (!m_shader.compile(SKY_VERT, SKY_FRAG)) return false;

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    return true;
}

void Skybox::draw(const Camera& camera, float aspect) {
    // Draw skybox with depth function set to LEQUAL so it draws
    // behind everything (since we set z = w in the vertex shader)
    glDepthFunc(GL_LEQUAL);

    m_shader.use();
    m_shader.setMat4("view", camera.getViewMatrix());
    m_shader.setMat4("projection", camera.getProjectionMatrix(aspect));

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Restore default depth function
    glDepthFunc(GL_LESS);
}

void Skybox::destroy() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    m_shader.destroy();
}