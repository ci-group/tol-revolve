from revolve.build.sdf import RobotBuilder, BodyBuilder, NeuralNetBuilder, BasicBattery
from revolve.angle.robogen.util import apply_surface_parameters
from sdfbuilder import SDF
from ..spec import get_body_spec, get_brain_spec


def get_builder(conf):
    """
    :param conf:
    :return:
    """
    body_spec = get_body_spec(conf)
    brain_spec = get_brain_spec(conf)
    return RobotBuilder(BodyBuilder(body_spec, conf), NeuralNetBuilder(brain_spec))


def get_simulation_robot(robot, name, builder, conf, battery_charge=None):
    """
    :param robot:
    :param name:
    :param builder:
    :param conf: Config
    :param battery_charge:
    :return:
    """
    battery = None if battery_charge is None else BasicBattery(battery_charge)
    model = builder.get_sdf_model(robot, controller_plugin="libtolrobotcontrol.so",
                                  update_rate=conf.controller_update_rate, name=name,
                                  battery=battery)

    apply_surface_parameters(model, conf.world_step_size)

    sdf = SDF()
    sdf.add_element(model)
    return sdf

