<?php

use yii\helpers\Html;


/* @var $this yii\web\View */
/* @var $model app\models\Hculightstrdatainfo */

$this->title = 'Create Hculightstrdatainfo';
$this->params['breadcrumbs'][] = ['label' => 'Hculightstrdatainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hculightstrdatainfo-create">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
