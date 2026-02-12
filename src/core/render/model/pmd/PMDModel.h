#pragma once

#include "../../../collider/OBB.h"
#include "../../texrure/Texture.h"
#include "../Model.h"
#include "material/PMDMaterialData.h"
#include "vertex/PMDVertexData.h"

struct ID3D11Device;
class PMDModelLoader;
class PMDMorphManager;
class MMDPhysics;
class PMDBoneManager;
class IKSolver;
class VMDMotionManager;

class PMDModel : public Model {
private:
    std::shared_ptr<MMDPhysics> physics; // モデル一体に物理世界を割り当てる(複数用にsharedは可能にする)
    std::shared_ptr<std::vector<PMDVertexData>> vertices;
    std::shared_ptr<PMDBoneManager> bone_manager;
    std::shared_ptr<PMDMorphManager> morph_manager;
    std::shared_ptr<PMDModelLoader> model_loader;
    std::shared_ptr<VMDMotionManager> motion_manager;
    std::shared_ptr<IKSolver> ik_soulver;
    std::vector<PMDMaterialData> materials;
    std::vector<Texture> toon_textures;
    Microsoft::WRL::ComPtr<ID3D11BlendState> blend_state;

public:
    explicit PMDModel(const std::filesystem::path& path);

    bool init(ID3D11Device* const device) override;
    void render_update(ID3D11DeviceContext* const context) override;
    void render(ID3D11DeviceContext* const context, const ShaderBindingSlots* slots) const override;
    bool is_loaded_model(void) override;
    void unload_model(void) override;

    void compute_obb(std::unordered_map<short, OBB>& obb_map) override;
    void update_obb(std::unordered_map<short, OBB>& obb_map) override;
    void on_clicked(const short obb_index) override;

    void update_motion(const DeltaTime& delta_time) override;

private:

    bool load_pmd(void);
    bool make_buffers(ID3D11Device* const device);
    bool make_vertex_buffer(ID3D11Device* const device);
    bool make_index_buffer(ID3D11Device* const device);
    bool make_material_buffer(ID3D11Device* const device);
    bool make_blend_state(ID3D11Device* const device);
    bool make_rigid_body(void);
    bool make_joint(void);
};
