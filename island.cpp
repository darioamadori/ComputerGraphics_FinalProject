// This has been adapted from the Vulkan tutorial

#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"


std::vector<SingleText> outText = {
        {1, {"Computer Graphics Final Project"}, 0, 0},
};

// The uniform buffer object used in this example
#define NSHIP 16
#define ANIMALS_NUM 5
#define BEAR 0
#define BISON 1
#define CAMEL 2
#define CROCODILE 3
#define ELEPHANT 4

struct BlinnUniformBufferObject {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};

struct BlinnUniformBufferObjectMultiple {
    alignas(16) glm::mat4 mvpMat[5];
    alignas(16) glm::mat4 mMat[5];
    alignas(16) glm::mat4 nMat[5];
};

struct skyBoxUniformBufferObject {
    alignas(16) glm::mat4 mvpMat;
};

struct BlinnMatParUniformBufferObject {
    alignas(4)  float Power;
};

struct EmissionUniformBufferObject {
    alignas(16) glm::mat4 mvpMat;
};

struct GlobalUniformBufferObject {
    alignas(16) glm::vec3 lightDir;
    alignas(16) glm::vec4 lightColor;
    alignas(16) glm::vec3 eyePos;
};

// **A10** Place here the CPP struct for the uniform buffer for the matrices
struct EarthMatricesUniformBufferObject {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};

// **A10** Place here the CPP struct for the uniform buffer for the parameters
struct EarthParametersUniformBufferObject {
    alignas(4) float Power;
    alignas(4) float Ang;
    alignas(4) float ShowCloud;
    alignas(4) float ShowTexture;
};




// The vertices data structures
struct BlinnVertex {
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec2 UV;
};

struct EmissionVertex {
    glm::vec3 pos;
    glm::vec2 UV;
};

struct skyBoxVertex {
    glm::vec3 pos;
};

// **A10** Place here the CPP struct for the vertex definition
struct EarthVertex {
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec2 UV;
    glm::vec4 tan;
};

glm::vec3 currentPos[ANIMALS_NUM] = {
        glm::vec3(-4, 0, 0),  // BEAR
        glm::vec3(4, 0, 0),   // BISON
        glm::vec3(2, 4, 0),   // CAMEL
        glm::vec3(0, 0, 0),   // CROCODILE
        glm::vec3(-2, 0, 0)   // ELEPHANT
};

// MAIN !
class A10 : public BaseProject {
protected:

    // Descriptor Layouts ["classes" of what will be passed to the shaders]
    DescriptorSetLayout DSLGlobal;	// For Global values

    DescriptorSetLayout DSLBlinn;	// For Blinn Objects
    DescriptorSetLayout DSLEmission;	// For Emission Objects
    DescriptorSetLayout DSLskyBox;	// For skyBox

// **A10** Place here the variable for the DescriptorSetLayout
    DescriptorSetLayout DSLEarth;

    // Vertex formats
    VertexDescriptor VDBlinn;
    VertexDescriptor VDEmission;
    VertexDescriptor VDskyBox;
// **A10** Place here the variable for the VertexDescriptor
    VertexDescriptor VDEarth;

    // Pipelines [Shader couples]
    Pipeline PBlinn;
    Pipeline PEmission;
    Pipeline PskyBox;
// **A10** Place here the variable for the Pipeline
    Pipeline PEarth;

    // Scenes and texts
    TextMaker txt;
    Texture noTexture;

    // Models, textures and Descriptor Sets (values assigned to the uniforms)
    DescriptorSet DSGlobal;

    Model Mship;
    Texture Tship;
    DescriptorSet DSship;

    Model Misland;
    Texture Tisland;
    Texture Tgrass;
    Texture Twood;
    DescriptorSet DSisland;

    Model Mcroc;
    Model Mwood;
    DescriptorSet DScroc;
    DescriptorSet DSwood;
    Model Mbear;
    DescriptorSet DSbear;
    Model Mbison;
    DescriptorSet DSbison;
    Model Mcamel;
    DescriptorSet DScamel;
    Model Melep;
    DescriptorSet DSelep;

    Model MRock1;
    DescriptorSet DSRock1[5];

    Model Mgrass;
    DescriptorSet DSgrass[50];

    Model MTree1;
    DescriptorSet DSTree1[10];
    Model MTree2;
    DescriptorSet DSTree2[4];


    Model Msun;
    Texture Tsun;
    DescriptorSet DSsun;

    Model MskyBox;
    Texture TskyBox, Tstars;
    DescriptorSet DSskyBox;

// **A10** Place here the variables for the Model, the five texture (diffuse, specular, normal map, emission and clouds) and the Descrptor Set
    Model MEarth;
    Texture TEarthDiffuse, TEarthSpecular, TEarthNormal, TEarthEmission, TEarthClouds;
    DescriptorSet DSEarth;


    // Other application parameters
    int currScene = 0;
    int subpass = 0;

/*    glm::vec3 CamPos = glm::vec3(0.0, 0.1, 5.0);
    glm::mat4 ViewMatrix;*/

    float Ar;

    // Here you set the main application parameters
    void setWindowParameters() {
        // window size, titile and initial background
        windowWidth = 800;
        windowHeight = 600;
        windowTitle = "A10 - Adding an object";
        windowResizable = GLFW_TRUE;
        initialBackgroundColor = {0.1f, 0.1f, 0.1f, 1.0f};

        Ar = (float)windowWidth / (float)windowHeight;
    }

    // What to do when the window changes size
    void onWindowResize(int w, int h) {
        std::cout << "Window resized to: " << w << " x " << h << "\n";
        Ar = (float)w / (float)h;
    }

    // Here you load and setup all your Vulkan Models and Texutures.
    // Here you also create your Descriptor set layouts and load the shaders for the pipelines
    void localInit() {
        // Descriptor Layouts [what will be passed to the shaders]
        DSLGlobal.init(this, {
                {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObject), 1}
        });
        DSLBlinn.init(this, {
                {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(BlinnUniformBufferObject), 1},
                {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
                {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(BlinnMatParUniformBufferObject), 1}
        });
        DSLEmission.init(this, {
                {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(EmissionUniformBufferObject), 1},
                {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1}
        });
        DSLskyBox.init(this, {
                {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(skyBoxUniformBufferObject), 1},
                {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
                {2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1, 1}
        });
// **A10** Place here the initialization of the the DescriptorSetLayout
        DSLEarth.init(this, {
                {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(EarthMatricesUniformBufferObject), 1},
                {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
                {2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1, 1},
                {3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 2, 1},
                {4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 3, 1},
                {5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 4, 1},
                {6, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(EarthParametersUniformBufferObject), 1}
        });

        // Vertex descriptors
        VDBlinn.init(this, {
                {0, sizeof(BlinnVertex), VK_VERTEX_INPUT_RATE_VERTEX}
        }, {
                             {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(BlinnVertex, pos),
                                     sizeof(glm::vec3), POSITION},
                             {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(BlinnVertex, norm),
                                     sizeof(glm::vec3), NORMAL},
                             {0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(BlinnVertex, UV),
                                     sizeof(glm::vec2), UV}
                     });
        VDEmission.init(this, {
                {0, sizeof(EmissionVertex), VK_VERTEX_INPUT_RATE_VERTEX}
        }, {
                                {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(EmissionVertex, pos),
                                        sizeof(glm::vec3), POSITION},
                                {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(EmissionVertex, UV),
                                        sizeof(glm::vec2), UV}
                        });
        VDskyBox.init(this, {
                {0, sizeof(skyBoxVertex), VK_VERTEX_INPUT_RATE_VERTEX}
        }, {
                              {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(skyBoxVertex, pos),
                               sizeof(glm::vec3), POSITION}
                      });
// **A10** Place here the initialization for the VertexDescriptor
        VDEarth.init(this, {
                {0, sizeof(EarthVertex), VK_VERTEX_INPUT_RATE_VERTEX}
        }, {
                             {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(EarthVertex, pos), sizeof(glm::vec3), POSITION},
                             {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(EarthVertex, norm), sizeof(glm::vec3), NORMAL},
                             {0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(EarthVertex, UV), sizeof(glm::vec2), UV},
                             {0, 3, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(EarthVertex, tan), sizeof(glm::vec4), TANGENT}
                     });

        // Pipelines [Shader couples]
        PBlinn.init(this, &VDBlinn,  "shaders/BlinnVert.spv",    "shaders/BlinnFrag.spv", {&DSLGlobal, &DSLBlinn});
        PEmission.init(this, &VDEmission,  "shaders/EmissionVert.spv",    "shaders/EmissionFrag.spv", {&DSLEmission});
        PskyBox.init(this, &VDskyBox, "shaders/SkyBoxVert.spv", "shaders/SkyBoxFrag.spv", {&DSLskyBox});
        PskyBox.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
                                    VK_CULL_MODE_BACK_BIT, false);
// **A10** Place here the initialization of the pipeline. Remember that it should use shaders in files
//		"shaders/NormalMapVert.spv" and "shaders/NormalMapFrag.spv", it should receive the new VertexDescriptor you defined
//		And should receive two DescriptorSetLayout, the first should be DSLGlobal, while the other must be the one you defined
        PEarth.init(this, &VDEarth, "shaders/NormalMapVert.spv", "shaders/NormalMapFrag.spv", {&DSLGlobal, &DSLEarth});

        // Create models
        Mship.init(this, &VDBlinn, "models/X-WING-baker.obj", OBJ);
        Misland.init(this, &VDBlinn, "models/grass.obj", OBJ);
        Mcroc.init(this, &VDBlinn, "models/crocodile_001.mgcg", MGCG);

        Mwood.init(this, &VDBlinn, "models/wood.obj", OBJ);
        Mbear.init(this, &VDBlinn, "models/bear_001.mgcg", MGCG);
        Mbison.init(this, &VDBlinn, "models/bison_001.mgcg", MGCG);
        Melep.init(this, &VDBlinn, "models/elephant_001.mgcg", MGCG);
        Mcamel.init(this, &VDBlinn, "models/camel_002.mgcg", MGCG);
        MRock1.init(this, &VDBlinn, "models/vegetation.049.mgcg", MGCG);
        Mgrass.init(this, &VDBlinn, "models/grass new.obj", OBJ);

        MTree1.init(this, &VDBlinn, "models/vegetation.003.mgcg", MGCG);
        MTree2.init(this, &VDBlinn, "models/vegetation.027.mgcg", MGCG);

        Msun.init(this, &VDEmission, "models/Sphere.obj", OBJ);
        MskyBox.init(this, &VDskyBox, "models/SkyBoxCube.obj", OBJ);
// **A10** Place here the loading of the model. It should be contained in file "models/Sphere.gltf", it should use the
//		Vertex descriptor you defined, and be of GLTF format.
        MEarth.init(this, &VDEarth, "models/Sphere.gltf", GLTF);

        // Create the textures
        Tship.init(this, "textures/XwingColors.png");
        Tisland.init(this,   "textures/grass2.jpg");
        Tgrass.init(this,   "textures/9.png");
        Twood.init(this,   "textures/wood.jpg");
        noTexture.init(this,   "textures/Textures-Animals.png");
        Tsun.init(this, "textures/2k_sun.jpg");
        TskyBox.init(this, "textures/1772.jpg");
        Tstars.init(this, "textures/constellation_figures.png");
// **A10** Place here the loading of the four textures
        // Diffuse color of the planet in: "2k_earth_daymap.jpg"

        // Specular color of the planet in: "2k_earth_specular_map.png"

        // Normal map of the planet in: "2k_earth_normal_map.png"
        // note that it must add a special feature to support the normal map, in particular
        // the init function should be the following: .init(this, "textures/2k_earth_normal_map.png", VK_FORMAT_R8G8B8A8_UNORM);

        // Emission map in: "2k_earth_nightmap.jpg"

        // Clouds map in: "2k_earth_clouds.jpg"
        TEarthDiffuse.init(this, "textures/2k_earth_daymap.jpg");
        TEarthSpecular.init(this, "textures/2k_earth_specular_map.png");
        TEarthNormal.init(this, "textures/2k_earth_normal_map.png", VK_FORMAT_R8G8B8A8_UNORM);
        TEarthEmission.init(this, "textures/2k_earth_nightmap.jpg");
        TEarthClouds.init(this, "textures/2k_earth_clouds.jpg");


        // Descriptor pool sizes
        // WARNING!!!!!!!!
        // Must be set before initializing the text and the scene
// **A10** Update the number of elements to correctly size the descriptor sets pool
        DPSZs.uniformBlocksInPool = 200;
        DPSZs.texturesInPool = 200;
        DPSZs.setsInPool = 100;


        std::cout << "Initializing text\n";
        txt.init(this, &outText);

        std::cout << "Initialization completed!\n";
        std::cout << "Uniform Blocks in the Pool  : " << DPSZs.uniformBlocksInPool << "\n";
        std::cout << "Textures in the Pool        : " << DPSZs.texturesInPool << "\n";
        std::cout << "Descriptor Sets in the Pool : " << DPSZs.setsInPool << "\n";

//        ViewMatrix = glm::translate(glm::mat4(1), -CamPos);
    }

    // Here you create your pipelines and Descriptor Sets!
    void pipelinesAndDescriptorSetsInit() {
        // This creates a new pipeline (with the current surface), using its shaders
        PBlinn.create();
        PEmission.create();
        PskyBox.create();
// **A10** Add the pipeline creation
        PEarth.create();

        // Here you define the data set
        DSship.init(this, &DSLBlinn, {&Tship});
        DSisland.init(this, &DSLBlinn, {&Tisland});
        DScroc.init(this, &DSLBlinn, {&noTexture});
        DSwood.init(this, &DSLBlinn, {&Twood});
        DSbear.init(this, &DSLBlinn, {&noTexture});
        DSbison.init(this, &DSLBlinn, {&noTexture});
        DScamel.init(this, &DSLBlinn, {&noTexture});
        DSelep.init(this, &DSLBlinn, {&noTexture});

        for(int i = 0; i< 5; i++){
            DSRock1[i].init(this, &DSLBlinn, {&noTexture});

        }
        for(int i = 0; i< 50; i++){
            DSgrass[i].init(this, &DSLBlinn, {&Tgrass});
        }

        for(int i = 0; i< 10; i++){
            DSTree1[i].init(this, &DSLBlinn, {&noTexture});

        }
        for(int i = 0; i< 4; i++){
            DSTree2[i].init(this, &DSLBlinn, {&noTexture});

        }

        DSsun.init(this, &DSLEmission, {&Tsun});
        DSskyBox.init(this, &DSLskyBox, {&TskyBox, &Tstars});
// **A10** Add the descriptor set creation
// Textures should be passed in the diffuse, specular, normal map, emission and clouds order.
        DSEarth.init(this, &DSLEarth, {&TEarthDiffuse, &TEarthSpecular, &TEarthNormal, &TEarthEmission, &TEarthClouds});

        DSGlobal.init(this, &DSLGlobal, {});

        txt.pipelinesAndDescriptorSetsInit();
    }

    // Here you destroy your pipelines and Descriptor Sets!
    // All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
    void pipelinesAndDescriptorSetsCleanup() {
        // Cleanup pipelines
        PBlinn.cleanup();
        PEmission.cleanup();
        PskyBox.cleanup();
// **A10** Add the pipeline cleanup
        PEarth.cleanup();

        DSship.cleanup();
        DSisland.cleanup();
        DScroc.cleanup();
        DSwood.cleanup();
        DSbear.cleanup();
        DSbison.cleanup();
        DScamel.cleanup();
        DSelep.cleanup();

        for(int i = 0; i< 5; i++){
            DSRock1[i].cleanup();}
        for(int i = 0; i< 50; i++){
            DSgrass[i].cleanup();}
        for(int i = 0; i< 10; i++){
            DSTree1[i].cleanup();}
        for(int i = 0; i< 4; i++){
            DSTree2[i].cleanup();}
        DSsun.cleanup();
        DSskyBox.cleanup();
        DSGlobal.cleanup();
// **A10** Add the descriptor set cleanup
        DSEarth.cleanup();

        txt.pipelinesAndDescriptorSetsCleanup();
    }

    // Here you destroy all the Models, Texture and Desc. Set Layouts you created!
    // All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
    // You also have to destroy the pipelines: since they need to be rebuilt, they have two different
    // methods: .cleanup() recreates them, while .destroy() delete them completely
    void localCleanup() {
        Tship.cleanup();
        Tisland.cleanup();
        noTexture.cleanup();
        Mship.cleanup();
        Misland.cleanup();
        Mcroc.cleanup();
        Mwood.cleanup();
        Mbear.cleanup();
        Mbison.cleanup();
        Mcamel.cleanup();
        Melep.cleanup();
        MRock1.cleanup();
        Mgrass.cleanup();
        MTree1.cleanup();
        MTree2.cleanup();


        Tsun.cleanup();
        Msun.cleanup();

        TskyBox.cleanup();
        Tstars.cleanup();
        MskyBox.cleanup();
// **A10** Add the cleanup for models and textures
        TEarthDiffuse.cleanup();
        TEarthSpecular.cleanup();
        TEarthNormal.cleanup();
        TEarthEmission.cleanup();
        TEarthClouds.cleanup();
        MEarth.cleanup();


        // Cleanup descriptor set layouts
        DSLBlinn.cleanup();
        DSLEmission.cleanup();
        DSLGlobal.cleanup();
        DSLskyBox.cleanup();
// **A10** Add the cleanup for the descriptor set layout
        DSLEarth.cleanup();

        // Destroies the pipelines
        PBlinn.destroy();
        PEmission.destroy();
        PskyBox.destroy();
// **A10** Add the cleanup for the pipeline
        PEarth.destroy();

        txt.localCleanup();
    }

    // Here it is the creation of the command buffer:
    // You send to the GPU all the objects you want to draw,
    // with their buffers and textures

    void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
        // binds the pipeline
        PBlinn.bind(commandBuffer);

        // The models (both index and vertex buffers)
        Mship.bind(commandBuffer);
        Misland.bind(commandBuffer);

        // The descriptor sets, for each descriptor set specified in the pipeline
        DSGlobal.bind(commandBuffer, PBlinn, 0, currentImage);	// The Global Descriptor Set (Set 0)
        DSship.bind(commandBuffer, PBlinn, 1, currentImage);	// The Material and Position Descriptor Set (Set 1)

        // The actual draw call.
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(Mship.indices.size()), NSHIP, 0, 0, 0);

        DSisland.bind(commandBuffer, PBlinn, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(Misland.indices.size()), 1, 0, 0, 0);

        DScroc.bind(commandBuffer, PBlinn, 1, currentImage);
        Mcroc.bind(commandBuffer);
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(Mcroc.indices.size()), 1, 0, 0, 0);

        DSwood.bind(commandBuffer, PBlinn, 1, currentImage);
        Mwood.bind(commandBuffer);
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(Mwood.indices.size()), 1, 0, 0, 0);

        DSbear.bind(commandBuffer, PBlinn, 1, currentImage);
        Mbear.bind(commandBuffer);
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(Mbear.indices.size()), 1, 0, 0, 0);

        DScamel.bind(commandBuffer, PBlinn, 1, currentImage);
        Mcamel.bind(commandBuffer);
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(Mcamel.indices.size()), 1, 0, 0, 0);

        DSbison.bind(commandBuffer, PBlinn, 1, currentImage);
        Mbison.bind(commandBuffer);
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(Mbison.indices.size()), 1, 0, 0, 0);

        DSelep.bind(commandBuffer, PBlinn, 1, currentImage);
        Melep.bind(commandBuffer);
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(Melep.indices.size()), 1, 0, 0, 0);

        for(int i = 0; i< 5; i++){
            DSRock1[i].bind(commandBuffer, PBlinn, 1, currentImage);
            MRock1.bind(commandBuffer);
            vkCmdDrawIndexed(commandBuffer,
                             static_cast<uint32_t>(MRock1.indices.size()), 1, 0, 0, 0);}
        for(int i = 0; i< 50; i++){
            DSgrass[i].bind(commandBuffer, PBlinn, 1, currentImage);
            Mgrass.bind(commandBuffer);
            vkCmdDrawIndexed(commandBuffer,
                             static_cast<uint32_t>(Mgrass.indices.size()), 1, 0, 0, 0);}
        for(int i = 0; i< 10; i++){
            DSTree1[i].bind(commandBuffer, PBlinn, 1, currentImage);
            MTree1.bind(commandBuffer);
            vkCmdDrawIndexed(commandBuffer,
                             static_cast<uint32_t>(MTree1.indices.size()), 1, 0, 0, 0);}
        for(int i = 0; i< 4; i++){
            DSTree2[i].bind(commandBuffer, PBlinn, 1, currentImage);
            MTree2.bind(commandBuffer);
            vkCmdDrawIndexed(commandBuffer,
                             static_cast<uint32_t>(MTree2.indices.size()), 1, 0, 0, 0);}



        PEmission.bind(commandBuffer);
        Msun.bind(commandBuffer);
        DSsun.bind(commandBuffer, PEmission, 0, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(Msun.indices.size()), 1, 0, 0, 0);


        PskyBox.bind(commandBuffer);
        MskyBox.bind(commandBuffer);
        DSskyBox.bind(commandBuffer, PskyBox, 0, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(MskyBox.indices.size()), 1, 0, 0, 0);

// **A10** Add the commands to bind the pipeline, the mesh its two descriptor setes, and the draw call of the planet
        PEarth.bind(commandBuffer);
        MEarth.bind(commandBuffer);
        DSGlobal.bind(commandBuffer, PEarth, 0, currentImage);
        DSEarth.bind(commandBuffer, PEarth, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(MEarth.indices.size()), 1, 0, 0, 0);


        txt.populateCommandBuffer(commandBuffer, currentImage, currScene);
    }
    std::vector<glm::vec2> bisonWaypoints = { {0.0f, 0.0f}, {12.0f, 5.0f} }; // Waypoints per il bisonte
    std::vector<glm::vec2> bearWaypoints = { {-3.0f, -1.0f}, {-13.0f, -2.0f} }; // Waypoints per l'orso
    std::vector<glm::vec2> camelWaypoints = { {3.0f, 2.0f}, {13.0f, 7.0f} }; // Waypoints per il cammello
    std::vector<glm::vec2> crocodileWaypoints = { {0.0f, -12.0f}, {0.0f, 12.0f} }; // Waypoints per il coccodrillo

    int currentWaypointIndex_Bison = 0; // Indice del waypoint corrente per il bisonte
    int currentWaypointIndex_Bear = 0; // Indice del waypoint corrente per l'orso
    int currentWaypointIndex_Camel = 0; // Indice del waypoint corrente per il cammello
    int currentWaypointIndex_Crocodile = 0; // Indice del waypoint corrente per il coccodrillo

    float bisonSpeed = 0.005f; // Velocità del bisonte
    float bearSpeed = 0.01f; // Velocità dell'orso
    float camelSpeed = 0.004f; // Velocità del cammello
    float crocodileSpeed = 0.05; // Velocità del coccodrillo

    float crocRotation = 0.0f;
    // Variabili globali per la telecamera
    glm::vec3 camPos = glm::vec3(0, 0, 3);
    glm::vec3 camFront = glm::vec3(0, 0, -1); // Direzione iniziale della telecamera
    glm::vec3 camUp = glm::vec3(0, 1, 0);
    glm::vec3 center = glm::vec3(0, 0, 0);
    glm::mat4 View;
    // Here is where you update the uniforms.
    // Very likely this will be where you will be writing the logic of your application.
    void updateUniformBuffer(uint32_t currentImage) {

        // Standard procedure to quit when the ESC key is pressed
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        float deltaT;
        glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
        bool fire = false;
        getSixAxis(deltaT, m, r, fire);

        static float autoTime = true;
        static float cTime = 0.0;
        const float turnTime = 72.0f;
        const float angTurnTimeFact = 2.0f * M_PI / turnTime;

        if(autoTime) {
            cTime = cTime + deltaT;
            cTime = (cTime > turnTime) ? (cTime - turnTime) : cTime;
        }

        static float tTime = 0.0;
        const float TturnTime = 60.0f;
        const float TangTurnTimeFact = 1.0f / TturnTime;

        if(autoTime) {
            tTime = tTime + deltaT;
            tTime = (tTime > TturnTime) ? (tTime - TturnTime) : tTime;
        }

// Parameters
        // Camera FOV-y, Near Plane and Far Plane
        const float FOVy = glm::radians(90.0f);
        const float nearPlane = 0.1f;
        const float farPlane = 100.0f;

        glm::mat4 Prj = glm::perspective(FOVy, Ar, nearPlane, farPlane);
        Prj[1][1] *= -1;

// Calculate camera movement based on input
        const float cameraSpeed = 4.0f * deltaT; // Adjust the speed according to your needs
        const float sensitivity = 2.0f; // Sensitivity of rotation

        // Update camera orientation based on arrow keys input
        static float yaw = -90.0f; // Yaw is initialized to -90.0 degrees to look along the negative z-axis by default
        static float pitch = 0.0f;

        if (glfwGetKey(window, GLFW_KEY_UP)) {
            pitch += sensitivity;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN)) {
            pitch -= sensitivity;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT)) {
            yaw -= sensitivity;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
            yaw += sensitivity;
        }

        // Limit pitch to avoid gimbal lock
        if (pitch > 89.0f) {
            pitch = 89.0f;
        }
        if (pitch < -89.0f) {
            pitch = -89.0f;
        }

        // Prevent the user to exit from the island


        // Calculate new front vector for the camera direction
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        camFront = glm::normalize(front);

        // Calculate new front vector for the movement
        glm::vec3 moveFront;
        moveFront.x = cos(glm::radians(yaw));
        moveFront.y = 0;
        moveFront.z = sin(glm::radians(yaw));
        moveFront = glm::normalize(moveFront);

        // Update camPos based on user input (keyboard or gamepad)
        if (glfwGetKey(window, GLFW_KEY_W)) {
            camPos += moveFront * cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_S)) {
            camPos -= moveFront * cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_A)) {
            camPos -= glm::normalize(glm::cross(moveFront, camUp)) * cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D)) {
            camPos += glm::normalize(glm::cross(moveFront, camUp)) * cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_R)) {
            camPos += camUp * cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_F)) {
            camPos -= camUp * cameraSpeed;
        }

        // Prevent the camera from going below the floor
        camPos.y = 1.5f;

        // Prevent the camera from leaving the island boundaries (circular island with radius 12.5)
        float radius = 13.0f;
        glm::vec2 camPosXZ = glm::vec2(camPos.x, camPos.z); // Current camera position on the XZ plane
        if (glm::length(camPosXZ) > radius) {
            camPosXZ = glm::normalize(camPosXZ) * radius; // Restrict the camera to the edge of the circle
            camPos.x = camPosXZ.x;
            camPos.z = camPosXZ.y;
        }


        std::vector<std::pair<int, int>> positions1(10);
        positions1[0] = std::make_pair(10, 5);
        positions1[1] = std::make_pair(-15, 20);
        positions1[2] = std::make_pair(-5, -10);
        positions1[3] = std::make_pair(7, -20);
        positions1[4] = std::make_pair(-25, 5);
        positions1[5] = std::make_pair(0, 25);

        std::vector<std::pair<int, int>> positions2(4);
        positions2[0] = std::make_pair(7, 1);
        positions2[1] = std::make_pair(-5, 9);
        positions2[2] = std::make_pair(-5, -10);
        positions2[3] = std::make_pair(7, -10);

        std::vector<std::pair<int, int>> positions3(5);
        positions3[0] = std::make_pair(7, 10);
        positions3[1] = std::make_pair(-5, 2);
        positions3[2] = std::make_pair(-10, -5);
        positions3[3] = std::make_pair(7, -10);
        positions3[4] = std::make_pair(-2, 5);

        // Update View matrix
        View = glm::lookAt(camPos, camPos + camFront, camUp);

        // Here is where you actually update your uniforms
        glm::mat4 M = glm::perspective(glm::radians(45.0f), Ar, 0.1f, 160.0f);
        M[1][1] *= -1;

        glm::mat4 Mv = View;

        glm::mat4 ViewPrj =  M * Mv;
        glm::mat4 baseTr = glm::mat4(1.0f);

        // updates global uniforms
        // Global
        GlobalUniformBufferObject gubo{};
        gubo.lightDir = glm::vec3(cos(glm::radians(135.0f)) * cos(cTime * angTurnTimeFact), sin(glm::radians(135.0f)), cos(glm::radians(135.0f)) * sin(cTime * angTurnTimeFact));
        gubo.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        gubo.eyePos = glm::vec3(glm::inverse(View) * glm::vec4(0, 0, 0, 1));
        DSGlobal.map(currentImage, &gubo, 0);

        // objects
        BlinnUniformBufferObject blinnUbo{};
        BlinnMatParUniformBufferObject blinnMatParUbo{};

        // ISLAND
        blinnUbo.mMat = glm::rotate(glm::translate(glm::mat4(1), glm::vec3(0, -1.0, 0)) *
                                    glm::scale(glm::mat4(1), glm::vec3(0.1f)),glm::radians(-90.0f), glm::vec3(1, 0, 0) );
        blinnUbo.mvpMat = ViewPrj * blinnUbo.mMat;
        blinnUbo.nMat = glm::inverse(glm::transpose(blinnUbo.mMat));
        DSisland.map(currentImage, &blinnUbo, 0);
        blinnMatParUbo.Power = 200.0;
        DSisland.map(currentImage, &blinnMatParUbo, 2);

        // WOOD
        blinnUbo.mMat = glm::scale(glm::translate(glm::rotate(
                glm::rotate(glm::scale(glm::mat4(1), glm::vec3(1.0f)), glm::radians(0.0f), glm::vec3(1, 0, 0)),
                glm::radians(180.0f), glm::vec3(0, 0, 1)),  glm::vec3 (0,-3,0.1)), glm::vec3(0.75, 1, 1.1) );
        blinnUbo.mvpMat = ViewPrj * blinnUbo.mMat;
        blinnUbo.nMat = glm::inverse(glm::transpose(blinnUbo.mMat));
        DSwood.map(currentImage, &blinnUbo, 0);
        DSwood.map(currentImage, &blinnMatParUbo, 2);

        // CROCODILE
        blinnUbo.mMat = glm::translate(glm::rotate(
                glm::rotate(glm::scale(glm::mat4(1), glm::vec3(1.0f)), glm::radians(90.0f), glm::vec3(1, 0, 0)),
                glm::radians(crocRotation), glm::vec3(0, 0, 1)), (currentPos[CROCODILE]));
        blinnUbo.mvpMat = ViewPrj * blinnUbo.mMat;
        blinnUbo.nMat = glm::inverse(glm::transpose(blinnUbo.mMat));
        DScroc.map(currentImage, &blinnUbo, 0);
        DScroc.map(currentImage, &blinnMatParUbo, 2);

        // BISON
        blinnUbo.mMat = glm::translate(glm::rotate(
                glm::rotate(glm::scale(glm::mat4(1), glm::vec3(1.0f)), glm::radians(90.0f), glm::vec3(1, 0, 0)),
                glm::radians(180.0f), glm::vec3(0, 0, 1)), (currentPos[BISON]));
        blinnUbo.mvpMat = ViewPrj * blinnUbo.mMat;
        blinnUbo.nMat = glm::inverse(glm::transpose(blinnUbo.mMat));
        DSbison.map(currentImage, &blinnUbo, 0);
        DSbison.map(currentImage, &blinnMatParUbo, 2);

        // CAMEL
        blinnUbo.mMat = glm::translate(glm::rotate(
                glm::rotate(glm::scale(glm::mat4(1), glm::vec3(1.0f)), glm::radians(90.0f), glm::vec3(1, 0, 0)),
                glm::radians(180.0f), glm::vec3(0, 0, 1)), (currentPos[CAMEL]));
        blinnUbo.mvpMat = ViewPrj * blinnUbo.mMat;
        blinnUbo.nMat = glm::inverse(glm::transpose(blinnUbo.mMat));
        DScamel.map(currentImage, &blinnUbo, 0);
        DScamel.map(currentImage, &blinnMatParUbo, 2);

        //BEAR
        blinnUbo.mMat = glm::translate(glm::rotate(
                glm::rotate(glm::scale(glm::mat4(1), glm::vec3(1.0f)), glm::radians(90.0f), glm::vec3(1, 0, 0)),
                glm::radians(180.0f), glm::vec3(0, 0, 1)), (currentPos[BEAR]));
        blinnUbo.mvpMat = ViewPrj * blinnUbo.mMat;
        blinnUbo.nMat = glm::inverse(glm::transpose(blinnUbo.mMat));
        DSbear.map(currentImage, &blinnUbo, 0);
        DSbear.map(currentImage, &blinnMatParUbo, 2);

        // ELEPHANT
        blinnUbo.mMat = glm::translate(glm::rotate(
                glm::rotate(glm::scale(glm::mat4(1), glm::vec3(1.0f)), glm::radians(90.0f), glm::vec3(1, 0, 0)),
                glm::radians(180.0f), glm::vec3(0, 0, 1)), (currentPos[ELEPHANT]));
        blinnUbo.mvpMat = ViewPrj * blinnUbo.mMat;
        blinnUbo.nMat = glm::inverse(glm::transpose(blinnUbo.mMat));
        DSelep.map(currentImage, &blinnUbo, 0);
        DSelep.map(currentImage, &blinnMatParUbo, 2);

        // ANIMALS MOVEMENTS:
        // CROCODILE
        glm::vec2 directionCrocodile = crocodileWaypoints[currentWaypointIndex_Crocodile] - glm::vec2(currentPos[CROCODILE].x, currentPos[CROCODILE].y);
        float distanceCrocodile = glm::length(directionCrocodile);
        if (distanceCrocodile > 0.01f) {
            glm::vec2 moveDir = glm::normalize(directionCrocodile);
            currentPos[CROCODILE].x += moveDir.x * crocodileSpeed;
            currentPos[CROCODILE].y += moveDir.y * crocodileSpeed;
        }
        if (glm::length(crocodileWaypoints[currentWaypointIndex_Crocodile] - glm::vec2(currentPos[CROCODILE].x, currentPos[CROCODILE].y)) < 0.01f) {
            // gira di 180 gradi TODO: calcolare l'angolo di girata con tangente
            if (crocRotation == 0.0f)
                crocRotation = 180.0f;
            else if (crocRotation == 180.0f)
                crocRotation = 0.0f;
            currentWaypointIndex_Crocodile++;
            if (currentWaypointIndex_Crocodile >= crocodileWaypoints.size()) {
                currentWaypointIndex_Crocodile = 0;
            }
        }
        // end animals movements



        for(int j = 0; j < 5; j++) {

            blinnUbo.mMat = glm::translate(
                    glm::rotate(
                            glm::rotate(
                                    glm::scale(glm::mat4(1), glm::vec3(1.0f)),
                                    glm::radians(180.0f), glm::vec3(1,0,0)
                            ),
                            glm::radians(180.0f), glm::vec3(0,0,1)
                    ),

                    glm::vec3(positions3[j].first, 0, positions3[j].second)
            );
            blinnUbo.mvpMat = ViewPrj * blinnUbo.mMat;
            blinnUbo.nMat = glm::inverse(glm::transpose(blinnUbo.mMat));
            DSRock1[j].map(currentImage, &blinnUbo, 0);
            DSRock1[j].map(currentImage, &blinnMatParUbo, 2);
            //ubo9.nMat[j] = glm::inverse(glm::transpose(ubo9.mMat[j]));
            // Update buffer for each iteration


            // Call rendering function for each object if needed
        }

        for(int j = 0; j < 6; j++) {

            blinnUbo.mMat = glm::translate(
                    glm::rotate(
                            glm::rotate(
                                    glm::scale(glm::mat4(1), glm::vec3(0.5f)),
                                    glm::radians(180.0f), glm::vec3(1, 0, 0)
                            ),
                            glm::radians(180.0f), glm::vec3(0, 0, 1)
                    ),

                    glm::vec3(positions1[j].first, 0, positions1[j].second)

            );
            blinnUbo.mvpMat = ViewPrj * blinnUbo.mMat;
            blinnUbo.nMat = glm::inverse(glm::transpose(blinnUbo.mMat));
            DSTree1[j].map(currentImage, &blinnUbo, 0);
            DSTree1[j].map(currentImage, &blinnMatParUbo, 2);
            //ubo9.nMat[j] = glm::inverse(glm::transpose(ubo9.mMat[j]));
            // Update buffer for each iteration
        }

        for(int j = 0; j < 4; j++) {

            blinnUbo.mMat = glm::translate(
                    glm::rotate(
                            glm::rotate(
                                    glm::scale(glm::mat4(1), glm::vec3(1.0f)),
                                    glm::radians(180.0f), glm::vec3(1,0,0)
                            ),
                            glm::radians(180.0f), glm::vec3(0,0,1)
                    ),

                    glm::vec3(positions2[j].first, 0, positions2[j].second)
            );
            blinnUbo.mvpMat = ViewPrj * blinnUbo.mMat;
            blinnUbo.nMat = glm::inverse(glm::transpose(blinnUbo.mMat));
            DSTree2[j].map(currentImage, &blinnUbo, 0);
            DSTree2[j].map(currentImage, &blinnMatParUbo, 2);


            // Call rendering function for each object if needed
        }

        for(int j = 0; j < 6; j++) {

            blinnUbo.mMat = glm::translate(
                    glm::rotate(
                            glm::rotate(
                                    glm::scale(glm::mat4(1), glm::vec3(1.0f)),
                                    glm::radians(180.0f), glm::vec3(1,0,0)
                            ),
                            glm::radians(180.0f), glm::vec3(0,0,1)
                    ),

                    glm::vec3(5, 0, 5)

            );
            blinnUbo.mvpMat = ViewPrj * blinnUbo.mMat;
            blinnUbo.nMat = glm::inverse(glm::transpose(blinnUbo.mMat));
            DSgrass[j].map(currentImage, &blinnUbo, 0);
            DSgrass[j].map(currentImage, &blinnMatParUbo, 2);
            //ubo9.nMat[j] = glm::inverse(glm::transpose(ubo9.mMat[j]));
            // Update buffer for each iteration


            // Call rendering function for each object if needed
        }


        EmissionUniformBufferObject emissionUbo{};
        emissionUbo.mvpMat = ViewPrj * glm::translate(glm::mat4(1), gubo.lightDir * 40.0f) * baseTr;
        DSsun.map(currentImage, &emissionUbo, 0);

        skyBoxUniformBufferObject sbubo{};
        sbubo.mvpMat = M * glm::mat4(glm::mat3(Mv));
        DSskyBox.map(currentImage, &sbubo, 0);


    }
};

// This is the main: probably you do not need to touch this!
int main() {
    A10 app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}