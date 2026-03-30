#include "Renderer.h"
#include <glad/glad.h>

static const char* VERT_SRC = R"(
    #version 330 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aNormal;
    layout(location = 2) in vec4 aColor; // Changed to vec4

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    out vec3 fragPos;
    out vec3 fragNormal;
    out vec4 fragColor; // Changed to vec4

    void main() {
        fragPos = vec3(model * vec4(aPos, 1.0));
        fragNormal = mat3(transpose(inverse(model))) * aNormal;
        fragColor = aColor;
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
)";

static const char* FRAG_SRC = R"(
    #version 330 core
    in vec3 fragPos;
    in vec3 fragNormal;
    in vec4 fragColor; // Changed to vec4
    out vec4 FragColor;

    #define MAX_LIGHTS 4
    uniform vec3 lightPositions[MAX_LIGHTS];
    uniform vec3 lightColors[MAX_LIGHTS];
    uniform int lightCount;
    uniform vec3 viewPos;

    uniform vec3 fogColor;
    uniform float fogStart;
    uniform float fogEnd;

    void main() {
        vec3 normal = normalize(fragNormal);
        vec3 viewDir = normalize(viewPos - fragPos);
        vec3 result = vec3(0.0);

        for (int i = 0; i < lightCount; i++) {
            // Treat light positions as infinitely far away (Directional Light)
            // By NOT subtracting fragPos, the light hits everything from the exact same angle!
            vec3 lightDir = normalize(lightPositions[i]); 
            
            // Base ambient light so shadows aren't pitch black
            vec3 ambient = 0.12 * lightColors[i];
            
            // Diffuse lighting
            float diff = max(dot(normal, lightDir), 0.0);
            vec3 diffuse = diff * lightColors[i];

            // Specular highlighting on terrain and meshes
            vec3 halfDir = normalize(lightDir + viewDir);
            float spec = pow(max(dot(normal, halfDir), 0.0), 32.0);
            vec3 specular = 0.3 * spec * lightColors[i];

            // Add the light perfectly uniformly, forever (No distance attenuation!)
            result += (ambient + diffuse + specular);
        }

        // Multiply lighting result by the RGB, keep the original alpha!
        result *= fragColor.rgb; 

        // Distance fog math stays the same
        float viewDist = length(viewPos - fragPos);
        float fogFactor = clamp((fogEnd - viewDist) / (fogEnd - fogStart), 0.0, 1.0);
        result = mix(fogColor, result, fogFactor);

        // Gamma correction
        result = pow(result, vec3(1.0 / 2.2));

        // Use the alpha channel we got from the vertex!
        FragColor = vec4(result, fragColor.a); 
    }
)";

bool Renderer::init() {
    if (!m_shader.compile(VERT_SRC, FRAG_SRC)) return false;
    if (!m_skybox.init()) return false;
    return true;
}

void Renderer::render(const Scene& scene, float aspect) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_skybox.draw(scene.camera, aspect);

    m_shader.use();

    glm::mat4 view = scene.camera.getViewMatrix();
    glm::mat4 proj = scene.camera.getProjectionMatrix(aspect);
    m_shader.setMat4("view", view);
    m_shader.setMat4("projection", proj);
    m_shader.setVec3("viewPos", scene.camera.position);

    m_shader.setVec3("fogColor", glm::vec3(0.6f, 0.7f, 0.85f));
    m_shader.setFloat("fogStart", 100.0f);
    m_shader.setFloat("fogEnd", 250.0f);

    int count = std::min((int)scene.lights.size(), 4);
    m_shader.setInt("lightCount", count);
    for (int i = 0; i < count; i++) {
        std::string pos = "lightPositions[" + std::to_string(i) + "]";
        std::string col = "lightColors[" + std::to_string(i) + "]";
        m_shader.setVec3(pos.c_str(), scene.lights[i].position);
        m_shader.setVec3(col.c_str(), scene.lights[i].color);
    }

    m_shader.setMat4("model", glm::mat4(1.0f));
    m_worldStreamer.draw();

    for (const auto& entity : scene.entities) {
        if (entity.mesh) {
            m_shader.setMat4("model", entity.transform.getMatrix());
            entity.mesh->draw();
        }
    }
}

void Renderer::shutdown() {
    m_shader.destroy();
    m_skybox.destroy();
    m_worldStreamer.destroy(); 
}